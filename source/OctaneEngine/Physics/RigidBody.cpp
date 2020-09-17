#include <OctaneEngine/Physics/RigidBody.h>
#include <OctaneEngine/Math.h>

namespace Octane
{

void RigidBody::Integrate(float dt)
{
  //integrate velocity
  //calculate linear
  DirectX::XMVECTOR linear_velocity = DirectX::XMVectorAdd(XMLoadFloat3(&linear_velocity_), DirectX::XMVectorScale(XMLoadFloat3(&force_accumulator_), mass_data_.inverse_mass * dt));
  //calculate angular
  DirectX::XMVECTOR angular_velocity = DirectX::XMVectorAdd(XMLoadFloat3(&angular_velocity_), DirectX::XMVectorScale(XMVector3Transform(XMLoadFloat3(&torque_accumulator_), XMLoadFloat3x3(&global_inverse_inertia_)), dt));
  //store velocity result
  XMStoreFloat3(&linear_velocity_, linear_velocity);
  XMStoreFloat3(&angular_velocity_, angular_velocity);
  //reset accumulator.
  force_accumulator_  = DirectX::XMFLOAT3();
  torque_accumulator_ = DirectX::XMFLOAT3();
  //integrate transformation
  //calculate linear
  DirectX::XMVECTOR delta_linear_velocity = HadamardProduct(DirectX::XMVectorScale(linear_velocity, dt), linear_constraints_);
  DirectX::XMVECTOR global_centroid       = DirectX::XMVectorAdd(XMLoadFloat3(&global_centroid_), delta_linear_velocity);
  //calculate angular
  DirectX::XMVECTOR delta_angular_velocity = HadamardProduct(DirectX::XMVectorScale(angular_velocity, dt), angular_constraints_);
  DirectX::XMVECTOR axis                   = DirectX::XMVector3Normalize(delta_angular_velocity);
  float             radian                 = DirectX::XMVector3Length(delta_angular_velocity).m128_f32[0] * dt;
  DirectX::XMVECTOR orientation            = DirectX::XMQuaternionMultiply(XMLoadFloat4(&orientation_), DirectX::XMQuaternionRotationAxis(axis, radian));
  //store transformation result
  XMStoreFloat3(&global_centroid_, global_centroid);
  XMStoreFloat4(&orientation_, orientation);
}

}
