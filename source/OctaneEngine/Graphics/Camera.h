#pragma once

#include <OctaneEngine/Math.h>

namespace Octane
{
class SphericalCamera
{
public:
  SphericalCamera();
  ~SphericalCamera();

  DirectX::XMFLOAT4X4 GetViewMatrix() const;

  void LookAt(
    const DirectX::XMFLOAT3& position,
    const DirectX::XMFLOAT3& target,
    const DirectX::XMFLOAT3& up = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));

    void LookAt(
    const DirectX::XMVECTOR& position,
    const DirectX::XMVECTOR& target,
    const DirectX::XMVECTOR& up =DirectX::XMVectorSetY(DirectX::XMVECTOR(), 1.0f));



  //basis i = 0, j = 1, k = 2
  void AddPosition(float scale, int basis);

private:
  DirectX::XMMATRIX view_matrix_ = DirectX::XMMatrixIdentity();
  //vector3 for cartesian basis
  DirectX::XMVECTOR i_hat_ = DirectX::XMVectorSetX(DirectX::XMVECTOR(), 1.0f),
                    j_hat_ = DirectX::XMVectorSetY(DirectX::XMVECTOR(), 1.0f),
                    k_hat_ = DirectX::XMVectorSetZ(DirectX::XMVECTOR(), 1.0f);
  DirectX::XMVECTOR target_position_;
  DirectX::XMVECTOR camera_position_;
  DirectX::XMVECTOR orientation_ = DirectX::XMVectorSet(0, 0, 0, 1);

  //for spherical coordinate
  float theta_ = -Math::HALF_PI;
  float phi_ = Math::HALF_PI;
  float radius_ = 60.0f;

  float camera_speed_ = 1.0f;
};

class FPSCamera
{
public:
  explicit FPSCamera(DirectX::XMVECTOR position);

  void MoveRelativeToView(DirectX::XMVECTOR offset);
  void SetPosition(DirectX::XMVECTOR position);

  void RotatePitchRelative(float angle_offset);
  void RotateYawRelative(float angle_offset);

  void RotatePitchAbsolute(float angle);
  void RotateYawAbsolute(float angle);

  DirectX::XMVECTOR GetViewDirection();
  DirectX::XMMATRIX GetViewMatrix();

  DirectX::XMFLOAT3 GetPosition() const;

private:
  DirectX::XMFLOAT3 position_;
  DirectX::XMFLOAT3 forward_, up_, right_;
  DirectX::XMFLOAT4X4 view_matrix_;
  float pitch_, yaw_;
  bool position_changed_, orientation_changed_;
  bool Dirty() const;
  void RecalculateVectors();
};

} // namespace Octane
