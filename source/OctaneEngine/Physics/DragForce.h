#pragma once

#include <OctaneEngine/Physics/Force.h>

namespace Octane
{

class DragForce final : public IForce
{
public:
  DragForce();
  ~DragForce();

  void Solve(RigidBody* body, float dt) override;

private:
};

} // namespace Octane
