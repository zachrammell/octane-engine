#include <OctaneEngine/Math.h>
#include <OctaneEngine/Physics/RigidBody.h>

namespace Octane
{

void RigidBody::Integrate(float dt)
{
  //integrate velocity
  //calculate linear
  linear_velocity_
    = DirectX::XMVectorAdd(linear_velocity_, DirectX::XMVectorScale(force_accumulator_, mass_data_.inverse_mass * dt));
  //calculate angular
  angular_velocity_ = DirectX::XMVectorAdd(
    angular_velocity_,
    DirectX::XMVectorScale(XMVector3Transform(torque_accumulator_, global_inverse_inertia_), dt));
  //reset accumulator.
  force_accumulator_  = XMLoadFloat3(&zero_vector_);
  torque_accumulator_ = XMLoadFloat3(&zero_vector_);
  //integrate transformation
  //calculate linear
  DirectX::XMVECTOR delta_linear_velocity
    = HadamardProduct(DirectX::XMVectorScale(linear_velocity_, dt), linear_constraints_);
  global_centroid_ = DirectX::XMVectorAdd(global_centroid_, delta_linear_velocity);
  //calculate angular
  DirectX::XMVECTOR delta_angular_velocity
    = HadamardProduct(DirectX::XMVectorScale(angular_velocity_, dt), angular_constraints_);
  DirectX::XMVECTOR axis = DirectX::XMVector3Normalize(delta_angular_velocity);
  float radian           = DirectX::XMVector3Length(delta_angular_velocity).m128_f32[0] * dt;
  orientation_           = DirectX::XMQuaternionMultiply(orientation_, DirectX::XMQuaternionRotationAxis(axis, radian));
}

void RigidBody::ApplyForce(const DirectX::XMFLOAT3& force, const DirectX::XMFLOAT3& at)
{
  DirectX::XMVECTOR force_vec = XMLoadFloat3(&force);
  force_accumulator_          = DirectX::XMVectorAdd(force_accumulator_, force_vec);
  torque_accumulator_         = DirectX::XMVectorAdd(
    torque_accumulator_,
    DirectX::XMVector3Cross(DirectX::XMVectorSubtract(XMLoadFloat3(&at), global_centroid_), force_vec));
}

void RigidBody::ApplyForceCentroid(const DirectX::XMFLOAT3& force)
{
  force_accumulator_ = DirectX::XMVectorAdd(force_accumulator_, XMLoadFloat3(&force));
}

void RigidBody::ApplyTorque(const DirectX::XMFLOAT3& torque)
{
  torque_accumulator_ = DirectX::XMVectorAdd(torque_accumulator_, XMLoadFloat3(&torque));
}

void RigidBody::UpdateCentroid()
{
  global_centroid_
    = DirectX::XMVectorAdd(Rotate(inverse_orientation_, orientation_, mass_data_.local_centroid), position_);
}

void RigidBody::UpdatePosition()
{
  position_ = DirectX::XMVectorAdd(
    Rotate(inverse_orientation_, orientation_, DirectX::XMVectorNegate(mass_data_.local_centroid)),
    global_centroid_);
}

void RigidBody::UpdateInertia()
{
  DirectX::XMMATRIX orientation         = DirectX::XMMatrixRotationQuaternion(orientation_);
  DirectX::XMMATRIX inverse_orientation = DirectX::XMMatrixRotationQuaternion(inverse_orientation_);
  global_inverse_inertia_
    = XMMatrixMultiply(XMMatrixMultiply(orientation, mass_data_.local_inverse_inertia), inverse_orientation);
  global_inertia_ = XMMatrixMultiply(XMMatrixMultiply(orientation, mass_data_.local_inertia), inverse_orientation);
}

void RigidBody::UpdateOrientation()
{
  orientation_         = DirectX::XMQuaternionNormalize(orientation_);
  inverse_orientation_ = DirectX::XMQuaternionInverse(orientation_);
  inverse_orientation_ = DirectX::XMQuaternionNormalize(inverse_orientation_);
}

} // namespace Octane
