#pragma once
#include <EASTL/hash_map.h>
#include <EASTL/vector.h>
#include <OctaneEngine/Components/PhysicsComponent.h>
#include <OctaneEngine/Components/TransformComponent.h>
#include <OctaneEngine/ISystem.h>

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
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
  btGhostObject* CreateSensor(const btTransform& transform, btCollisionShape* shape) const;
 
private:
  btBroadphaseInterface* bt_broad_phase_ = nullptr;
  btDefaultCollisionConfiguration* bt_collision_config_ = nullptr;
  btCollisionDispatcher* bt_narrow_phase_ = nullptr;
  btSequentialImpulseConstraintSolver* bt_resolution_phase_ = nullptr;
  btDiscreteDynamicsWorld* bt_world_ = nullptr;
  btAlignedObjectArray<btCollisionShape*> collision_shapes_;
};
} // namespace Octane
