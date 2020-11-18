#pragma once
#include <OctaneEngine/Components/PhysicsComponent.h>
#include <OctaneEngine/Components/TransformComponent.h>
#include <OctaneEngine/ISystem.h>
#include <EASTL/hash_map.h>
#include <EASTL/vector.h>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <OctaneEngine/Physics/Shape.h>


namespace Octane
{
class PhysicsSys final : public ISystem
{
  // ISystem implementation
public:
  explicit PhysicsSys(Engine* engine);
  ~PhysicsSys() = default;

  void Load() override {};
  void LevelStart() override;
  void Update() override;
  void LevelEnd() override;
  void Unload() override {};
  static SystemOrder GetOrder();

public:

    void InitializePhysics(PhysicsComponent* physics_compo);

  //eCollisionState HasCollision(PhysicsComponent& a, PhysicsComponent& b) const;
  

private:
  btBroadphaseInterface* bt_broad_phase_ = nullptr;
  btDefaultCollisionConfiguration* bt_collision_config_ = nullptr;
  btCollisionDispatcher* bt_narrow_phase_ = nullptr;
  btSequentialImpulseConstraintSolver* bt_resolution_phase_ = nullptr;
  btDiscreteDynamicsWorld* bt_world_ = nullptr;
  btAlignedObjectArray<btCollisionShape*> collision_shapes_;
};
} // namespace Octane
