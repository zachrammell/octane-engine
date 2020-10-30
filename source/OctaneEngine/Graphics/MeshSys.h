#pragma once

#include <OctaneEngine/ISystem.h>
#include <OctaneEngine/Helper.h>
#include <OctaneEngine/Graphics/Mesh.h>
#include <OctaneEngine/Components/RenderComponent.h>

#include <EASTL/fixed_vector.h>


struct aiScene;
struct aiNode;
struct aiMesh;

namespace Octane
{
class MeshSys : public ISystem
{
public:
  enum Models
  {
    MFirst,
    MBear = MFirst,
    MDuck,
    MCrossbow,
    MPaperPlane,
    MShuriken,
    MPaperStack,

    MCount
  };
  explicit MeshSys(class Engine* parent_engine);
  ~MeshSys();
  //MeshSys(ID3D11Device* device, ID3D11DeviceContext* context);
  Model* GetModel(Models model);

  virtual void Load() {};
  virtual void LevelStart() {};
  virtual void Update() {};
  virtual void LevelEnd() {};
  virtual void Unload() {};

  static SystemOrder GetOrder();
  eastl::fixed_vector<MeshDX11, to_integral(MeshType::COUNT), false>& Meshes();

private:
  //Model* LoadModel(const char* path);
  //void ProcessNode(const aiScene* scene, aiNode* node, eastl::vector<MeshDX11>& meshes);
  //MeshDX11 ProcessMesh(const aiScene* scene, aiMesh* mesh);
  //eastl::map<Models, Model*> models;
  eastl::fixed_vector<MeshDX11, to_integral(MeshType::COUNT), false> meshes_;

  Mesh LoadMesh(const char* path);
  void ProcessNode(const aiScene* scene, aiNode* node, Mesh& mesh);
  void ProcessMesh(aiMesh* mesh, Mesh& new_mesh);
};

} // namespace Octane
