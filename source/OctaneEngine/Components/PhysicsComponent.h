#pragma once
#include <DirectXMath.h>
#include <OctaneEngine/EntityID.h>
#include <btBulletDynamicsCommon.h>

namespace Octane
{
class PhysicsComponent
{
public:
  void SetRotation(const DirectX::XMFLOAT4& rotation) const;
  void SetRotation(const DirectX::XMFLOAT3& euler_angle) const;
  void ApplyForce(const DirectX::XMFLOAT3& force) const;
  void ApplyTorque(const DirectX::XMFLOAT3& torque) const;
  void SetGravity(float force) const;

  friend class PhysicsSys;
  friend class ComponentSys;
  btRigidBody* rigid_body = nullptr;
  EntityID parent_entity = INVALID_ENTITY;

private:
  // position should only be set through transform
  void SetPosition(const DirectX::XMFLOAT3& position) const;
};
} // namespace Octane
