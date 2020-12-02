#pragma once
#include <btBulletDynamicsCommon.h>
#include <DirectXMath.h>

namespace Octane
{
class PhysicsComponent
{
public:
  friend class PhysicsSys;
  friend class ComponentSys;
  btRigidBody* rigid_body = nullptr;
  void SetRotation(DirectX::XMFLOAT4 quaternion_rotation);
};
} // namespace Octane
