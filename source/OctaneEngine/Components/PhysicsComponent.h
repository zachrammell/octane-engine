#pragma once
#include <btBulletDynamicsCommon.h>

namespace Octane
{
class PhysicsComponent
{
  friend class PhysicsSys;
  btRigidBody* rigid_body = nullptr;
};
} // namespace Octane
