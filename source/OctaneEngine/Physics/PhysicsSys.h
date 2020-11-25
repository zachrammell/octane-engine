#pragma once
#include <EASTL/map.h>
#include <OctaneEngine/Components/PhysicsComponent.h>
#include <OctaneEngine/Components/TransformComponent.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/ISystem.h>
#include <OctaneEngine/Physics/PhysicsDef.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

namespace Octane
{
class PhysicsSys final : public ISystem
{
  // ISystem implementation
public:
  explicit PhysicsSys(Engine* engine);
  ~PhysicsSys();

  void Load() override {};
  void LevelStart() override;
  void Update() override;
  void LevelEnd() override;
  void Unload() override {};

  static SystemOrder GetOrder();

public:
  void InitializePhysicsBox(
    PhysicsComponent* physics_compo,
    const DirectX::XMFLOAT3& box_half_size,
    const DirectX::XMFLOAT3& position,
    const DirectX::XMFLOAT4& rotation,

    bool is_dynamic,
    bool is_sensor = false);

  void SetPosition(PhysicsComponent* compo, const DirectX::XMFLOAT3& position);
  void ApplyForce(PhysicsComponent* compo, const DirectX::XMFLOAT3& force) const;
  void ApplyTorque(PhysicsComponent* compo, const DirectX::XMFLOAT3& torque) const;

  //eCollisionState HasCollision(PhysicsComponent& a, PhysicsComponent& b) const;

private:
  btRigidBody* CreateRigidBody(float mass, const btTransform& transform, btCollisionShape* shape) const;
  btRigidBody* CreateSensor(float mass, const btTransform& transform, btCollisionShape* shape) const;

  static void BulletCallback(btDynamicsWorld* world, btScalar time_step);
  static void BulletCollisionCallback(
    btBroadphasePair& collisionPair,
    btCollisionDispatcher& dispatcher,
    const btDispatcherInfo& dispatchInfo);

private:
  eastl::multimap<EntityID, EntityID> entity_collisions_;
  btBroadphaseInterface* broad_phase_;
  btDefaultCollisionConfiguration* collision_config_;
  btCollisionDispatcher* narrow_phase_;
  btSequentialImpulseConstraintSolver* resolution_phase_;
  btDiscreteDynamicsWorld* dynamics_world_;
  btAlignedObjectArray<btCollisionShape*> collision_shapes_;
};
} // namespace Octane
