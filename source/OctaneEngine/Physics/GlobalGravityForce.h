#pragma once

#include <OctaneEngine/Physics/Force.h>

namespace Octane
{

class GlobalGravityForce final : public IForce
{
public:
  explicit GlobalGravityForce(const DirectX::XMFLOAT3& gravity);
  ~GlobalGravityForce();

  void Solve(RigidBody& body, float dt) override;

private:
  DirectX::XMVECTOR gravity_;
};

} // namespace Octane
