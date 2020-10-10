#include <OctaneEngine/Graphics/Camera.h>

#include <OctaneEngine/Math.h>

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

FPSCamera::FPSCamera(DirectX::XMVECTOR position)
  : pitch_ {},
    yaw_ {},
    position_changed_ {true},
    orientation_changed_ {true}
{
  using namespace DirectX;
  XMStoreFloat3(&position_, position);
  XMStoreFloat3(&forward_, XMVectorSet(0, 0, 1, 0));
  XMStoreFloat3(&up_, XMVectorSet(0, 1, 0, 0));
  XMStoreFloat3(&right_, XMVectorSet(1, 0, 0, 0));
  XMStoreFloat4x4(&view_matrix_, XMMatrixIdentity());
}

void FPSCamera::MoveRelativeToView(DirectX::XMVECTOR offset)
{
  using namespace DirectX;
  position_changed_ = true;
  if (orientation_changed_)
  {
    RecalculateVectors();
    orientation_changed_ = false;
  }
  XMMATRIX change_of_basis = XMMatrixIdentity();
  change_of_basis.r[0] = XMLoadFloat3(&right_);
  change_of_basis.r[1] = XMLoadFloat3(&up_);
  change_of_basis.r[2] = XMLoadFloat3(&forward_);
  XMVECTOR const position = XMLoadFloat3(&position_);
  XMStoreFloat3(&position_, position + XMVector3Transform(offset, change_of_basis));
}

void FPSCamera::SetPosition(DirectX::XMVECTOR position)
{
  position_changed_ = true;
  XMStoreFloat3(&position_, position);
}

void FPSCamera::RotatePitchRelative(float angle_offset)
{
  orientation_changed_ = true;
  pitch_ = clamp(pitch_ + DirectX::XMConvertToRadians(angle_offset), -89.9f, 89.9f);
}

void FPSCamera::RotateYawRelative(float angle_offset)
{
  orientation_changed_ = true;
  yaw_ += DirectX::XMConvertToRadians(angle_offset);
}

void FPSCamera::RotatePitchAbsolute(float angle)
{
  orientation_changed_ = true;
  pitch_ = clamp(DirectX::XMConvertToRadians(angle), -89.9f, 89.9f);
}

void FPSCamera::RotateYawAbsolute(float angle)
{
  orientation_changed_ = true;
  yaw_ = DirectX::XMConvertToRadians(angle);
}

DirectX::XMVECTOR FPSCamera::GetViewDirection()
{
  if (orientation_changed_)
  {
    orientation_changed_ = false;
    RecalculateVectors();
  }

  return DirectX::XMLoadFloat3(&forward_);
}

DirectX::XMMATRIX FPSCamera::GetViewMatrix()
{
  using namespace DirectX;
  if (Dirty())
  {
    position_changed_ = orientation_changed_ = false;
    RecalculateVectors();
    XMVECTOR const position = XMLoadFloat3(&position_);
    // recalculate view matrix
    XMMATRIX const ret = XMMatrixLookAtRH(position, position + XMLoadFloat3(&forward_), XMLoadFloat3(&up_));
    XMStoreFloat4x4(&view_matrix_, ret);
    return ret;
  }
  return XMLoadFloat4x4(&view_matrix_);
}

DirectX::XMFLOAT3 FPSCamera::GetPosition() const
{
  return position_;
}

bool FPSCamera::Dirty() const
{
  return (position_changed_ || orientation_changed_);
}

void FPSCamera::RecalculateVectors()
{
  using namespace DirectX;
  XMVECTOR const forward = XMVector3Normalize(XMVectorSet(
    XMScalarCos(yaw_) * XMScalarCos(pitch_),
    XMScalarSin(pitch_),
    XMScalarSin(yaw_) * XMScalarCos(pitch_),
    1.0f));
  XMVECTOR const right = XMVector3Normalize(XMVector3Cross(forward, XMVectorSet(0, 1, 0, 1)));
  XMStoreFloat3(&forward_, forward);
  XMStoreFloat3(&right_, -right);
  XMStoreFloat3(&up_, XMVector3Normalize(XMVector3Cross(right, forward)));
}

} // namespace Octane
