#pragma once
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

namespace Octane
{
enum class eShapeType
{
  Box,
  Sphere,
  Hull
};

struct RigidBodyPair
{
  btRigidBody* a;
  btRigidBody* b;
};

}
