#include <OctaneEngine/Graphics/Camera.h>

namespace Octane
{
SphericalCamera::SphericalCamera() : target_position_(), camera_position_() {}

SphericalCamera::~SphericalCamera() {}

DirectX::XMFLOAT4X4 SphericalCamera::GetViewMatrix() const
{
  DirectX::XMFLOAT4X4 result;
  DirectX::XMStoreFloat4x4(&result, view_matrix_);
  return result;
}

void SphericalCamera::LookAt(
  const DirectX::XMFLOAT3& position,
  const DirectX::XMFLOAT3& target,
  const DirectX::XMFLOAT3& up)
{
  camera_position_ = XMLoadFloat3(&position);
  target_position_ = XMLoadFloat3(&target);
  DirectX::XMVECTOR up_vector = XMLoadFloat3(&up);
  view_matrix_ = DirectX::XMMatrixLookAtLH(camera_position_, target_position_, up_vector);

  k_hat_ = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(target_position_, camera_position_));
  i_hat_ = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(up_vector, k_hat_));
  j_hat_ = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(k_hat_, i_hat_));

  orientation_ = Math::QuaternionFromTo(DirectX::XMVectorSetX(DirectX::XMVECTOR(), 1.0f), i_hat_);
}

void SphericalCamera::LookAt(
  const DirectX::XMVECTOR& position,
  const DirectX::XMVECTOR& target,
  const DirectX::XMVECTOR& up)
{
  camera_position_ = position;
  target_position_ = target;
  view_matrix_ = DirectX::XMMatrixLookAtLH(camera_position_, target_position_, up);

  k_hat_ = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(target_position_, camera_position_));
  i_hat_ = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(up, k_hat_));
  j_hat_ = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(k_hat_, i_hat_));

  orientation_ = Math::QuaternionFromTo(DirectX::XMVectorSetX(DirectX::XMVECTOR(), 1.0f), i_hat_);
}

void SphericalCamera::AddPosition(float scale, int basis)
{
  DirectX::XMVECTOR direction;
  int index = basis % 3;
  if (index == 0)
  {
    direction = i_hat_;
  }
  else if (index == 1)
  {
    direction = j_hat_;
  }
  else
  {
    direction = k_hat_;
  }

  DirectX::XMVECTOR changed = DirectX::XMVectorScale(direction, camera_speed_ * scale * radius_);
  target_position_ = DirectX::XMVectorAdd(target_position_, changed);
  camera_position_ = DirectX::XMVectorAdd(camera_position_, changed);

  LookAt(camera_position_, target_position_);
}

} // namespace Octane
