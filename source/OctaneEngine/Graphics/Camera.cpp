#include <OctaneEngine/Graphics/Camera.h>

#include <OctaneEngine/Math.h>

namespace dx = DirectX;

namespace Octane
{
SphericalCamera::SphericalCamera() : target_position_(), camera_position_() {}

SphericalCamera::~SphericalCamera() {}

dx::XMFLOAT4X4 SphericalCamera::GetViewMatrix() const
{
  dx::XMFLOAT4X4 result;
  dx::XMStoreFloat4x4(&result, view_matrix_);
  return result;
}

void SphericalCamera::LookAt(
  const dx::XMFLOAT3& position,
  const dx::XMFLOAT3& target,
  const dx::XMFLOAT3& up)
{
  camera_position_ = XMLoadFloat3(&position);
  target_position_ = XMLoadFloat3(&target);
  dx::XMVECTOR up_vector = XMLoadFloat3(&up);
  view_matrix_ = dx::XMMatrixLookAtLH(camera_position_, target_position_, up_vector);

  k_hat_ = dx::XMVector3Normalize(dx::XMVectorSubtract(target_position_, camera_position_));
  i_hat_ = dx::XMVector3Normalize(dx::XMVector3Cross(up_vector, k_hat_));
  j_hat_ = dx::XMVector3Normalize(dx::XMVector3Cross(k_hat_, i_hat_));

  orientation_ = Math::QuaternionFromTo(dx::XMVectorSetX(dx::XMVECTOR(), 1.0f), i_hat_);
}

void SphericalCamera::LookAt(
  const dx::XMVECTOR& position,
  const dx::XMVECTOR& target,
  const dx::XMVECTOR& up)
{
  camera_position_ = position;
  target_position_ = target;
  view_matrix_ = dx::XMMatrixLookAtLH(camera_position_, target_position_, up);

  k_hat_ = dx::XMVector3Normalize(dx::XMVectorSubtract(target_position_, camera_position_));
  i_hat_ = dx::XMVector3Normalize(dx::XMVector3Cross(up, k_hat_));
  j_hat_ = dx::XMVector3Normalize(dx::XMVector3Cross(k_hat_, i_hat_));

  orientation_ = Math::QuaternionFromTo(dx::XMVectorSetX(dx::XMVECTOR(), 1.0f), i_hat_);
}

void SphericalCamera::AddPosition(float scale, int basis)
{
  dx::XMVECTOR direction;
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

  dx::XMVECTOR changed = dx::XMVectorScale(direction, camera_speed_ * scale * radius_);
  target_position_ = dx::XMVectorAdd(target_position_, changed);
  camera_position_ = dx::XMVectorAdd(camera_position_, changed);

  LookAt(camera_position_, target_position_);
}

FPSCamera::FPSCamera(dx::XMVECTOR position)
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

void FPSCamera::MoveRelativeToView(dx::XMVECTOR offset)
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

void FPSCamera::SetPosition(dx::XMVECTOR position)
{
  position_changed_ = true;
  XMStoreFloat3(&position_, position);
}

void FPSCamera::RotatePitchRelative(float angle_offset)
{
  // TODO: call RotatePitchAbsolute instead of copy paste implementation
  orientation_changed_ = true;
  pitch_ = clamp(pitch_ + dx::XMConvertToRadians(angle_offset), -dx::XM_PIDIV2 + 0.00001f, dx::XM_PIDIV2 - 0.00001f);
}

void FPSCamera::RotateYawRelative(float angle_offset)
{
  orientation_changed_ = true;
  yaw_ += dx::XMConvertToRadians(angle_offset);
}

void FPSCamera::RotatePitchAbsolute(float angle)
{
  orientation_changed_ = true;
  pitch_ = clamp(dx::XMConvertToRadians(angle), -dx::XM_PIDIV2 + 0.00001f, dx::XM_PIDIV2 - 0.00001f);
}

void FPSCamera::RotateYawAbsolute(float angle)
{
  orientation_changed_ = true;
  yaw_ = dx::XMConvertToRadians(angle);
}

dx::XMVECTOR FPSCamera::GetViewDirection()
{
  if (orientation_changed_)
  {
    orientation_changed_ = false;
    RecalculateVectors();
  }

  return dx::XMLoadFloat3(&forward_);
}

dx::XMMATRIX FPSCamera::GetViewMatrix()
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

dx::XMFLOAT3 FPSCamera::GetPosition() const
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
