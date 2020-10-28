#include <OctaneEngine/Physics/DragForce.h>

namespace Octane
{

DragForce::DragForce(float l_drag, float r_drag) : linear_drag(l_drag), angular_drag(r_drag) {}

DragForce::~DragForce() {}

void DragForce::Solve(RigidBody& body, float dt)
{
  if (body.IsDynamic())
  {
    body.SetLinearVelocity(DirectX::XMVectorScale(body.GetLinearVelocity(), powf(1.0f - linear_drag, dt)));
    body.SetAngularVelocity(DirectX::XMVectorScale(body.GetAngularVelocity(), powf(1.0f - angular_drag, dt)));
  }
}
} // namespace Octane
