#pragma once

#include <OctaneEngine/Physics/RigidBody.h>

namespace Octane
{
class IForce
{
public:
  IForce() {}
  virtual ~IForce() {}
  virtual void Solve(RigidBody* body, float dt) = 0;

private:
};
} // namespace Octane
