#pragma once
#include <btBulletDynamicsCommon.h>

namespace Octane
{
class PhysicsComponent
{
public:
  friend class PhysicsSys;
  friend class ComponentSys;
  btRigidBody* rigid_body = nullptr;
};
} // namespace Octane
