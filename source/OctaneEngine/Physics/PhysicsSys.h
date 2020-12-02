#pragma once
#include <EASTL/hash_map.h>
#include <EASTL/vector.h>
#include <EASTL/set.h>
#include <EASTL/map.h>
#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Components/PhysicsComponent.h>
#include <OctaneEngine/Components/TransformComponent.h>
#include <OctaneEngine/ISystem.h>
#include <OctaneEngine/Physics/PhysicsDef.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

namespace Octane
{

//using CollisionPair = eastl::pair<const btRigidBody*, const btRigidBody*>;
//using CollisionKeyTable = eastl::hash_multimap<btRigidBody*, btRigidBody*>;
//using CollisionDataTable = eastl::hash_multimap<size_t, CollisionData>;

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

  using CollisionsResult = eastl::pair<
    eastl::multimap<ComponentHandle, ComponentHandle>::iterator,
    eastl::multimap<ComponentHandle, ComponentHandle>::iterator>;

  // returns a begin and end iterator into the multimap of collisions
  // for every (k,v) pair between the iterators, k = phys_component and v = one object that collided with it
  // if the two iterators are equal there are no collisions
  CollisionsResult GetCollisions(ComponentHandle phys_component);
  // returns true if there were any collisions with that component this frame
  bool HasCollisions(ComponentHandle phys_component) const;
  bool HasCollision(ComponentHandle lhs, ComponentHandle rhs);

public:
  void SetPosition(PhysicsComponent* compo, const DirectX::XMFLOAT3& position);
  void ApplyForce(PhysicsComponent* compo, const DirectX::XMFLOAT3& force) const;
  void ApplyTorque(PhysicsComponent* compo, const DirectX::XMFLOAT3& torque) const;
  DirectX::XMVECTOR GetVelocity(const PhysicsComponent* compo) const;

  //eCollisionState HasCollision(PhysicsComponent& a, PhysicsComponent& b) const;

  // used by the MakePhysics functions in ComponentSys
  btRigidBody* CreateRigidBody(float mass, const btTransform& transform, btCollisionShape* shape) const;
  btRigidBody* CreateSensor(float mass, const btTransform& transform, btCollisionShape* shape) const;
  void RemoveRigidBody(btRigidBody* rigid_body);

private:
  static void BulletCallback(btDynamicsWorld* world, btScalar time_step);
  static void BulletCollisionCallback(
    btBroadphasePair& collisionPair,
    btCollisionDispatcher& dispatcher,
    const btDispatcherInfo& dispatchInfo);

  eastl::multimap<ComponentHandle, ComponentHandle> entity_collisions_;
  btBroadphaseInterface* broad_phase_;
  btDefaultCollisionConfiguration* collision_config_;
  btCollisionDispatcher* narrow_phase_;
  btSequentialImpulseConstraintSolver* resolution_phase_;
  btDiscreteDynamicsWorld* dynamics_world_;
  btAlignedObjectArray<btCollisionShape*> collision_shapes_;
  //CollisionKeyTable collision_key_table_;
  //CollisionDataTable collision_data_table_;
};
} // namespace Octane
