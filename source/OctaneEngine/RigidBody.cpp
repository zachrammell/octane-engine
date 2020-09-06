#include <OctaneEngine/RigidBody.h>

#include <OctaneEngine/Math.h>

namespace Octane
{

void RigidBody::Integrate(float dt)
{
  // integrate linear velocity
  DirectX::XMVECTOR linear_velocity       = XMLoadFloat3(&linear_velocity_);
  DirectX::XMVECTOR force_accumulator     = XMLoadFloat3(&force_accumulator_);
  linear_velocity                         = DirectX::XMVectorAdd(linear_velocity, DirectX::XMVectorScale(force_accumulator, mass_data_.inverse_mass * dt));
  DirectX::XMVECTOR global_centroid       = XMLoadFloat3(&global_centroid_);
  DirectX::XMVECTOR delta_linear_velocity = HadamardProduct(DirectX::XMVectorScale(linear_velocity, dt), linear_constraints_);
  global_centroid                         = DirectX::XMVectorAdd(global_centroid, delta_linear_velocity);
  force_accumulator_                      = DirectX::XMFLOAT3();
  XMStoreFloat3(&linear_velocity_, linear_velocity);
  XMStoreFloat3(&global_centroid_, global_centroid);
}

}
