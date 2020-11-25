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

enum class eCollisionState : int
{
  None,
  Start,
  Persist,
  End
};

struct CollisionData
{
  CollisionData() {}

  CollisionData(const btRigidBody* a_, const btRigidBody* b_) : a(a_), b(b_) {}

  CollisionData(const btRigidBody* a_, const btRigidBody* b_, eCollisionState s) : a(a_), b(b_), state(s) {}

  friend bool operator<(const CollisionData& x, const CollisionData& y)
  {
    return x.a < y.a || (!(y.a < x.a) && x.b < y.b);
  }

  const btRigidBody* a = nullptr;
  const btRigidBody* b = nullptr;
  eCollisionState state = eCollisionState::None;
};

struct RigidBodyPair
{
  btRigidBody* a = nullptr;
  btRigidBody* b = nullptr;

  friend bool operator<(const RigidBodyPair& x, const RigidBodyPair& y)
  {
    return x.a < y.a || (!(y.a < x.a) && x.b < y.b);
  }

  RigidBodyPair() {}
  RigidBodyPair(const RigidBodyPair& rhs) : a(rhs.a), b(rhs.b) {}
};

} // namespace Octane