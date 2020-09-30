#pragma once
#include <OctaneEngine/ISystem.h>
#include <OctaneEngine/Physics/RigidBody.h>

#include <EASTL/vector.h>

namespace Octane
{
class World final : public ISystem
{
  // ISystem implementation
public:
  explicit World(Engine* engine);
  ~World() = default;

  void Load() override {};
  void LevelStart() override;
  void Update() override;
  void LevelEnd() override;
  void Unload() override {};

  static SystemOrder GetOrder();

public:
  RigidBody* GetRigidBody(size_t index);
  RigidBody* AddRigidBody();

private:

private:
  eastl::vector<RigidBody> rigid_bodies_;

};
}
