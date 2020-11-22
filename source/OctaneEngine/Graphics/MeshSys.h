#pragma once

#include <OctaneEngine/ISystem.h>
#include <OctaneEngine/Helper.h>
#include <OctaneEngine/Graphics/Mesh.h>
#include <OctaneEngine/Components/RenderComponent.h>

//#include <EASTL/fixed_vector.h>
#include <EASTL/hash_map.h>
#include <EASTL/shared_ptr.h>

struct aiScene;
struct aiNode;
struct aiMesh;

#define NEW_MESH_IMPLEMENTATION //super temporary to test old vs new implementation

namespace Octane
{

  typedef eastl::shared_ptr<MeshDX11> MeshPtr;

class MeshSys : public ISystem
{
public:

  explicit MeshSys(class Engine* parent_engine);
  ~MeshSys();
  //MeshSys(ID3D11Device* device, ID3D11DeviceContext* context);

  virtual void Load() {};
  virtual void LevelStart() {};
  virtual void Update() {};
  virtual void LevelEnd() {};
  virtual void Unload() {};

  static SystemOrder GetOrder();

  
#ifndef NEW_MESH_IMPLEMENTATION
  eastl::fixed_vector<MeshDX11, to_integral(MeshType::COUNT), false>& Meshes();

private:
  eastl::fixed_vector<MeshDX11, to_integral(MeshType::COUNT), false> meshes_;
#endif
#ifdef NEW_MESH_IMPLEMENTATION
  eastl::hash_map<Mesh_Key, MeshPtr>& Meshes();

private:
  eastl::hash_map<Mesh_Key, MeshPtr> meshes;
#endif

  Mesh LoadMesh(const char* path);
  void ProcessNode(const aiScene* scene, aiNode* node, Mesh& mesh);
  void ProcessMesh(aiMesh* mesh, Mesh& new_mesh);
};

} // namespace Octane
