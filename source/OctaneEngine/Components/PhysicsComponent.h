#pragma once
#include <btBulletDynamicsCommon.h>
#include <DirectXMath.h>

namespace Octane
{
class PhysicsComponent
{
public:
  void SetPosition(const DirectX::XMFLOAT3& position) const;
  void SetRotation(const DirectX::XMFLOAT4& rotation) const;
  void SetRotation(const DirectX::XMFLOAT3& euler_angle) const;
  void ApplyForce(const DirectX::XMFLOAT3& force) const;
  void ApplyTorque(const DirectX::XMFLOAT3& torque) const;
  void SetGravity(float force) const;

public:
  friend class PhysicsSys;
  friend class ComponentSys;
  btRigidBody* rigid_body = nullptr;
};
} // namespace Octane
