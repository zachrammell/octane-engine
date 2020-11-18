#pragma once
#include <btBulletDynamicsCommon.h>

namespace Octane
{
class PhysicsSys;

struct PhysicsComponent
{
  btRigidBody* rigid_body = nullptr;
  btCollisionShape* shape = nullptr;
  PhysicsSys* system = nullptr;
};
} // namespace Octane
