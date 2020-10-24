#define DEPRECATED
#ifndef DEPRECATED
#include <EASTL/string.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/Graphics/MeshSys.h>
#include <OctaneEngine/Graphics/RenderSys.h>
#include <OctaneEngine/SystemOrder.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <magic_enum.hpp>

namespace Octane
{
MeshSys::MeshSys(class Engine* parent_engine)
  : ISystem(parent_engine)
{

}

MeshSys::~MeshSys()
{
  for (int i = 0; i < models.size(); ++i)
  {
    if (models.at(Models(i)))
    {
      delete models.at(Models(i));
      models[Models(i)] = nullptr;
    }
  }
}

Model* MeshSys::GetModel(Models model)
{
  //if didnt find model, load it in
  if (models.find(model) == models.end())
  {
    models[model] = LoadModel(eastl::string {
      eastl::string {"./assets/models/"} + eastl::string {magic_enum::enum_name<Models>(model).data() + 1} + ".obj"}
                                .c_str());
  }

  return models[model];
}

SystemOrder MeshSys::GetOrder()
{
  return SystemOrder::Mesh;
}

Model* MeshSys::LoadModel(const char* path)
{
  Assimp::Importer importer;
  const aiScene* scene = nullptr; //= importer.ReadFile(path, aiProcess_Triangulate);
  eastl::vector<MeshDX11> meshes;

  if (scene)
  {
    ProcessNode(scene, scene->mRootNode, meshes);
  }

  return new Model {meshes};
}

void MeshSys::ProcessNode(const aiScene* scene, aiNode* node, eastl::vector<MeshDX11>& meshes)
{
  for (int i = 0; i < node->mNumMeshes; ++i)
  {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(ProcessMesh(scene, mesh));
  }

  for (int i = 0; i < node->mNumChildren; ++i)
  {
    ProcessNode(scene, node->mChildren[i], meshes);
  }
}

MeshDX11 MeshSys::ProcessMesh(const aiScene* scene, aiMesh* mesh)
{
  Mesh new_mesh;

  for (int i = 0; i < mesh->mNumVertices; ++i)
  {
    auto& mVert = mesh->mVertices[i];
    Mesh::Vertex vert {{mVert.x, mVert.y, mVert.z}, {}};

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
      new_mesh.index_buffer.push_back(face.mIndices[j]);
    }
  }

  return Get<RenderSys>()->GetGraphicsDeviceDX11().CreateMesh(new_mesh);
}

} // namespace Octane
#endif
#undef DEPRECATED