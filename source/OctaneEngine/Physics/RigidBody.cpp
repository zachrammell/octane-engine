#include <OctaneEngine/Math.h>
#include <OctaneEngine/Physics/RigidBody.h>

namespace Octane
{
RigidBody::RigidBody()
  : position_(),
    linear_velocity_(),
    force_accumulator_(),
    angular_velocity_(),
    torque_accumulator_(),
    mass_data_(),
    global_centroid_()
{
  inverse_orientation_ = DirectX::XMQuaternionInverse(orientation_);
  inverse_orientation_ = DirectX::XMQuaternionNormalize(inverse_orientation_);
}

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
  force_accumulator_ = DirectX::XMVECTOR();
  torque_accumulator_ = DirectX::XMVECTOR();
  //integrate transformation
  //calculate linear
  DirectX::XMVECTOR delta_linear_velocity
    = Math::HadamardProduct(DirectX::XMVectorScale(linear_velocity_, dt), linear_constraints_);
  global_centroid_ = DirectX::XMVectorAdd(global_centroid_, delta_linear_velocity);

  //calculate angular
  DirectX::XMVECTOR delta_angular_velocity
    = Math::HadamardProduct(DirectX::XMVectorScale(angular_velocity_, dt), angular_constraints_);
  DirectX::XMVECTOR axis = DirectX::XMVector3Normalize(delta_angular_velocity);
  float radian = DirectX::XMVector3Length(delta_angular_velocity).m128_f32[0] * dt;

  if (DirectX::XMVector3Length(axis).m128_f32[0] > 0.0f)
  {
    DirectX::XMVECTOR delta_orientation = DirectX::XMQuaternionRotationAxis(axis, radian);
    orientation_ = DirectX::XMQuaternionMultiply(orientation_, delta_orientation);
  }
}

void RigidBody::ApplyForce(const DirectX::XMFLOAT3& force, const DirectX::XMFLOAT3& at)
{
  DirectX::XMVECTOR force_vec = XMLoadFloat3(&force);
  force_accumulator_ = DirectX::XMVectorAdd(force_accumulator_, force_vec);
  torque_accumulator_ = DirectX::XMVectorAdd(
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
    = DirectX::XMVectorAdd(Math::Rotate(inverse_orientation_, orientation_, mass_data_.local_centroid), position_);
}

void RigidBody::UpdatePosition()
{
  position_ = DirectX::XMVectorAdd(
    Math::Rotate(inverse_orientation_, orientation_, DirectX::XMVectorNegate(mass_data_.local_centroid)),
    global_centroid_);
}

void RigidBody::UpdateInertia()
{
  DirectX::XMMATRIX orientation = DirectX::XMMatrixRotationQuaternion(orientation_);
  DirectX::XMMATRIX inverse_orientation = DirectX::XMMatrixRotationQuaternion(inverse_orientation_);
  global_inverse_inertia_
    = XMMatrixMultiply(XMMatrixMultiply(orientation, mass_data_.local_inverse_inertia), inverse_orientation);
  global_inertia_ = XMMatrixMultiply(XMMatrixMultiply(orientation, mass_data_.local_inertia), inverse_orientation);
}

void RigidBody::UpdateOrientation()
{
  orientation_ = DirectX::XMQuaternionNormalize(orientation_);
  inverse_orientation_ = DirectX::XMQuaternionInverse(orientation_);
  inverse_orientation_ = DirectX::XMQuaternionNormalize(inverse_orientation_);
}

void RigidBody::SetLinearConstraints(const DirectX::XMFLOAT3& linear_constraints)
{
  linear_constraints_ = XMLoadFloat3(&linear_constraints);
}

void RigidBody::SetAngularConstraints(const DirectX::XMFLOAT3& angular_constraints)
{
  angular_constraints_ = XMLoadFloat3(&angular_constraints);
}

void RigidBody::SetPosition(const DirectX::XMFLOAT3& position)
{
  position_ = XMLoadFloat3(&position);
  UpdateCentroid();
}

void RigidBody::SetOrientation(const DirectX::XMFLOAT4& orientation)
{
  orientation_ = XMLoadFloat4(&orientation);
  UpdateOrientation();
  UpdateCentroid();
  UpdateInertia();
}

void RigidBody::SyncToPosition(DirectX::XMFLOAT3& position) const
{
  XMStoreFloat3(&position, position_);
}

DirectX::XMFLOAT3 RigidBody::GetPosition() const
{
  DirectX::XMFLOAT3 pos;
  SyncToPosition(pos);
  return pos;
}

void RigidBody::SyncToOrientation(DirectX::XMFLOAT4& orientation) const
{
  XMStoreFloat4(&orientation, orientation_);
}

DirectX::XMFLOAT4 RigidBody::GetOrientation() const
{
  DirectX::XMFLOAT4 orientation;
  SyncToOrientation(orientation);
  return orientation;
}

DirectX::XMVECTOR RigidBody::LocalToWorldPoint(const DirectX::XMVECTOR& local_point) const
{
  return DirectX::XMVectorAdd(Math::Rotate(orientation_, local_point), position_);
}

DirectX::XMVECTOR RigidBody::WorldToLocalPoint(const DirectX::XMVECTOR& world_point) const
{
  return Math::Rotate(inverse_orientation_, DirectX::XMVectorSubtract(world_point, position_));
}

DirectX::XMVECTOR RigidBody::LocalToWorldVector(const DirectX::XMVECTOR& local_vector) const
{
  return Math::Rotate(orientation_, local_vector);
}

DirectX::XMVECTOR RigidBody::WorldToLocalVector(const DirectX::XMVECTOR& world_vector) const
{
  return Math::Rotate(inverse_orientation_, world_vector);
}

float RigidBody::Mass() const
{
  return mass_data_.mass;
}

DirectX::XMMATRIX RigidBody::Inertia() const
{
  return global_inertia_;
}

DirectX::XMVECTOR RigidBody::Centroid() const
{
  return global_centroid_;
}

DirectX::XMVECTOR RigidBody::GetLinearVelocity() const
{
  return linear_velocity_;
}

DirectX::XMVECTOR RigidBody::GetAngularVelocity() const
{
  return angular_velocity_;
}

void RigidBody::SetLinearVelocity(const DirectX::XMVECTOR& linear_velocity)
{
  linear_velocity_ = linear_velocity;
}

void RigidBody::SetAngularVelocity(const DirectX::XMVECTOR& angular_velocity)
{
  angular_velocity_ = angular_velocity;
}

} // namespace Octane
