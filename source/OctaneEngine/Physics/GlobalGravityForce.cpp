#include <OctaneEngine/Physics/GlobalGravityForce.h>

namespace Octane
{

GlobalGravityForce::GlobalGravityForce(const DirectX::XMFLOAT3& gravity)
{
  gravity_ = XMLoadFloat3(&gravity);
}

GlobalGravityForce::~GlobalGravityForce() {}

void GlobalGravityForce::Solve(RigidBody& body, float dt)
{
  /*if (body.IsDynamic())
  {
    body.SetLinearVelocity(DirectX::XMVectorAdd(body.GetLinearVelocity(), DirectX::XMVectorScale(gravity_, dt)));
  }*/
}
} // namespace Octane
