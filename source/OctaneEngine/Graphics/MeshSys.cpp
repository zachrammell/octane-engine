#include <OctaneEngine/Graphics/MeshSys.h>

#include <OctaneEngine/Engine.h>
#include <OctaneEngine/Graphics/RenderSys.h>
#include <OctaneEngine/SystemOrder.h>
#include <OctaneEngine/NBTReader.h>
#include <OctaneEngine/NBTWriter.h>
#include <OctaneEngine/Trace.h>
#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <iostream>
#include <fstream>

namespace dx = DirectX;

namespace Octane
{
MeshSys::MeshSys(class Engine* parent_engine) : ISystem(parent_engine)
{
  //NBTWriter write("assets/meshes.nbt");
  //write.WriteString("PathPrefix", "assets/models");
  
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
        meshnames_[i] = read.ReadString("name");
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

const MeshDX11* MeshSys::Get(Mesh_Key key)
{
  MeshPtr mesh = meshes_[key];

  if (mesh)
  {
    //has to be done this way to deref the shared_ptr
    return &*mesh;
  }
  
  auto& device = reinterpret_cast<RenderSys*>(engine_.GetSystem(SystemOrder::RenderSys))->GetGraphicsDeviceDX11();
  NBTReader read(datapath_);
  eastl::string path(path_);

  if(read.OpenList("Data"))
  {
    const int listSize = meshnames_.size();

    for(int i = 0; i < listSize; ++i)
    {
      if(read.OpenCompound(""))
      {
        if(read.ReadString("name") == key)
        {
          path.append(eastl::string {read.ReadString("path")});
          device.EmplaceMesh(meshes_, key, LoadMesh(path.data()));
          mesh = meshes_.find(key)->second;
        }
        read.CloseCompound();
      }
    }
    read.CloseList();
  }
  else
  {
    Trace::Log(Severity::WARNING, "MeshSys::Get, Failed to open list: Data");
  }

  if(mesh)
  {
    return &*mesh;  
  }

  Trace::Log(Severity::WARNING, "MeshSys::Get, Tried to load mesh: %s which doesn't exist in %s", path, datapath_);

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

  //Assimp::Importer importer;
  //const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_OptimizeMeshes);
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
    ProcessMesh(aimesh, mesh);
  }

  for (int i = 0; i < node->mNumChildren; ++i)
  {
    ProcessNode(scene, node->mChildren[i], mesh);
  }
}

void MeshSys::ProcessMesh(aiMesh* mesh, Mesh& new_mesh)
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
    // TODO: implement texture coordinates
#if 0
        if (mesh->mTextureCoords && mesh->mTextureCoords[0])
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
}

} // namespace Octane
