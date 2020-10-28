#pragma once

#include <OctaneEngine/Physics/Force.h>

namespace Octane
{

class DragForce final : public IForce
{
public:
  DragForce(float l_drag, float r_drag);
  ~DragForce();

  void Solve(RigidBody& body, float dt) override;

private:
  float linear_drag = 0.25f;
  float angular_drag = 0.25f;
};

} // namespace Octane
