#pragma once

#include <OctaneEngine/ISystem.h>
#include <OctaneEngine/Helper.h>
#include <OctaneEngine/Graphics/Mesh.h>
#include <OctaneEngine/Components/RenderComponent.h>

//#include <EASTL/fixed_vector.h>
#include <EASTL/hash_map.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/vector.h>
#include <EASTL/string.h>
struct aiScene;
struct aiNode;
struct aiMesh;


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
 
  const eastl::vector<eastl::string>& MeshNames() const;
  const MeshDX11* Get(Mesh_Key key);

private:
  eastl::hash_map<Mesh_Key, MeshPtr> meshes_;
  eastl::hash_map<eastl::string, eastl::string> meshToPath_; //maps mesh names to paths
  eastl::string_view datapath_{"assets/meshes.nbt"}; //path to meshes NBT
  eastl::string path_; //path to models
  eastl::vector<eastl::string> meshnames_;
  Mesh LoadMesh(const char* path);
  void ProcessNode(const aiScene* scene, aiNode* node, Mesh& mesh);
  void ProcessMesh(aiMesh* mesh, Mesh& new_mesh, const aiScene* scene);
};

} // namespace Octane
