#include <OctaneEngine/Graphics/MeshSys.h>

#include <OctaneEngine/Engine.h>
#include <OctaneEngine/Graphics/RenderSys.h>
#include <OctaneEngine/SystemOrder.h>
#include <OctaneEngine/NBTReader.h>
#include <OctaneEngine/Trace.h>
#include <OctaneEngine/Graphics/TextureSys.h>
#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>

namespace dx = DirectX;

namespace Octane
{
MeshSys::MeshSys(class Engine* parent_engine) : ISystem(parent_engine)
{  
  auto& device = reinterpret_cast<RenderSys*>(engine_.GetSystem(SystemOrder::RenderSys))->GetGraphicsDeviceDX11();
  NBTReader read(datapath_);
  
  path_ = read.ReadString("PathPrefix");
  if(path_.empty())
  {
    Trace::Log(Severity::ERROR, "MeshSys::MeshSys, failed to read path to meshes from meshes NBT\n");
  }

  if(read.OpenList("Data"))
  {
    const int meshListSize = read.ListSize();
    meshnames_.resize(meshListSize);
    for(int i = 0; i < meshListSize; ++i)
    {
      if(read.OpenCompound(""))
      {
        const eastl::string name{read.ReadString("name")};
        meshnames_[i] = name;
        meshToPath_[name] = path_ + eastl::string(read.ReadString("path"));
        read.CloseCompound();
      }
    }
    read.CloseList();
  }
}

MeshSys::~MeshSys() {}

SystemOrder MeshSys::GetOrder()
{
  return SystemOrder::MeshSys;
}

const eastl::vector<eastl::string>& MeshSys::MeshNames() const
{
  return meshnames_;
}

const MeshDX11* MeshSys::Get(Mesh_Key key) //todo: change this so it doesn't override Isystem::Get()
{
  MeshPtr mesh = meshes_[key];

  if (mesh)
  {
    //has to be done this way to deref the shared_ptr
    return &*mesh;
  }
  
  auto& device = reinterpret_cast<RenderSys*>(engine_.GetSystem(SystemOrder::RenderSys))->GetGraphicsDeviceDX11();

  device.EmplaceMesh(meshes_, key, LoadMesh(meshToPath_[eastl::string{key}].data()));
  mesh = meshes_.find(key)->second;

  if(mesh)
  {
    return &*mesh;  
  }

  Trace::Log(Severity::WARNING, "MeshSys::Get, Tried to load mesh: %s which doesn't exist in %s", key, path_);

  return nullptr;
}

Mesh MeshSys::LoadMesh(const char* path)
{
  aiPropertyStore* props = aiCreatePropertyStore();
  aiSetImportPropertyInteger(props, "PP_PTV_NORMALIZE", 1);
  const aiScene* scene = (aiScene*)aiImportFileExWithProperties(
    path,
    aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices
      | aiProcess_PreTransformVertices | aiProcess_OptimizeMeshes,
    NULL,
    props);
  aiReleasePropertyStore(props);

  Mesh new_mesh;
  if (scene)
  {
    ProcessNode(scene, scene->mRootNode, new_mesh);
  }

  return new_mesh;
}

void MeshSys::ProcessNode(const aiScene* scene, aiNode* node, Mesh& mesh)
{
  /*todo: load the scene with each mesh having its transformation from the scene
	instead of local space */
  for (int i = 0; i < node->mNumMeshes; ++i)
  {
    aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
    ProcessMesh(aimesh, mesh,scene);
  }

  for (int i = 0; i < node->mNumChildren; ++i)
  {
    ProcessNode(scene, node->mChildren[i], mesh);
  }
}

void MeshSys::ProcessMesh(aiMesh* mesh, Mesh& new_mesh,const aiScene* scene)
{
  size_t start = new_mesh.vertex_buffer.size(); //start of next mesh in aiScene
  const bool hasNormals = mesh->HasNormals();

  for (int i = 0; i < mesh->mNumVertices; ++i)
  {
    auto& mVert = mesh->mVertices[i];
    dx::XMFLOAT3 norm {};
    if (hasNormals)
    {
      norm = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
    }
    Mesh::Vertex vert {{mVert.x, mVert.y, mVert.z}, norm};
#if 1
    if (mesh->HasTextureCoords(0))
    {
      vert.uv.x = mesh->mTextureCoords[0][i].x;
      vert.uv.y = mesh->mTextureCoords[0][i].y;
    }
#endif
    new_mesh.vertex_buffer.push_back(vert);
  }
  for (int i = 0; i < mesh->mNumFaces; ++i)
  {
    auto& face = mesh->mFaces[i];
    for (int j = 0; j < face.mNumIndices; ++j)
    {
      new_mesh.index_buffer.push_back(face.mIndices[j] + start);
    }
  }

  //get textures from material
  aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
  if (mat)
  {
    
    auto diffTexCount = mat->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE);
    if(!diffTexCount)
    {
      new_mesh.material.diffuse_texture = UNTEXTURED;
    }
    else
    {
      for(UINT i = 0; i < diffTexCount; ++i)
      {
        aiString diffTexName;
        mat->GetTexture(aiTextureType::aiTextureType_DIFFUSE, i, &diffTexName);
        eastl::string_view texName {diffTexName.C_Str()};
        auto end = texName.find_last_of(".");
        auto begin1 = texName.find_last_of("/");
        auto begin2 = texName.find_last_of("\\");
        auto begin = eastl::min(begin1, begin2) + 1;
        eastl::string finalTexName{texName.data() + begin, texName.data() + end};
        if(auto texture = ISystem::Get<TextureSys>()->Get(finalTexName))
        {
          new_mesh.textures.push_back(finalTexName);
          new_mesh.material.diffuse_texture = texture->key;
        }
      }
      /*new_mesh.material.diffuse_texture = ISystem::Get<TextureSys>()->Get(*///RCAST(engine_.GetSystem(SystemOrder::TextureSys), TextureSys*);
    }
  }
}

} // namespace Octane
