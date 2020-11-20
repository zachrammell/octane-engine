#pragma once
#include <btBulletDynamicsCommon.h>

namespace Octane
{
class PhysicsComponent
{
  friend class PhysicsSys;
  btCollisionObject* collision_object = nullptr;
  PhysicsSys* system = nullptr;
};
} // namespace Octane
