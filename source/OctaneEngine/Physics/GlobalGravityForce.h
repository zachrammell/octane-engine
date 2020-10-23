#pragma once

#include <OctaneEngine/Physics/Force.h>

namespace Octane
{

class GlobalGravityForce final : public IForce
{
public:
  GlobalGravityForce();
  ~GlobalGravityForce();

  void Solve(RigidBody* body, float dt) override;

private:
};

} // namespace Octane
