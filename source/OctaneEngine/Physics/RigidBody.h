#pragma once

#include <DirectXMath.h>
#include <OctaneEngine/Physics/MassData.h>

namespace Octane
{
class RigidBody
{
public:
  RigidBody();
  ~RigidBody() = default;

  void Integrate(float dt);

  void ApplyForce(const DirectX::XMFLOAT3& force, const DirectX::XMFLOAT3& at);
  void ApplyForceCentroid(const DirectX::XMFLOAT3& force);
  void ApplyTorque(const DirectX::XMFLOAT3& torque);

  void UpdateCentroid();
  void UpdatePosition();
  void UpdateInertia();
  void UpdateOrientation();

  void SetLinearConstraints(const DirectX::XMFLOAT3& linear_constraints);
  void SetAngularConstraints(const DirectX::XMFLOAT3& angular_constraints);

  void SyncFromPosition(const DirectX::XMFLOAT3& position);
  void SyncFromOrientation(const DirectX::XMFLOAT4& orientation);
  void SyncToPosition(DirectX::XMFLOAT3& position) const;
  void SyncToOrientation(DirectX::XMFLOAT4& orientation) const;

  DirectX::XMVECTOR LocalToWorldPoint(const DirectX::XMVECTOR& local_point) const;
  DirectX::XMVECTOR WorldToLocalPoint(const DirectX::XMVECTOR& world_point) const;
  DirectX::XMVECTOR LocalToWorldVector(const DirectX::XMVECTOR& local_vector) const;
  DirectX::XMVECTOR WorldToLocalVector(const DirectX::XMVECTOR& world_vector) const;

private:
  //linear data - positional
  DirectX::XMVECTOR position_;                                                          //vector3 - for transform
  DirectX::XMVECTOR linear_velocity_;                                                   //vector3
  DirectX::XMVECTOR force_accumulator_;                                                 //vector3
  DirectX::XMVECTOR linear_constraints_ = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f); //vector3

  //angular data - rotational
  DirectX::XMVECTOR orientation_ = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);         //quaternion - for transform
  DirectX::XMVECTOR inverse_orientation_ = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f); //quaternion
  DirectX::XMVECTOR angular_velocity_;                                                   //vector3
  DirectX::XMVECTOR torque_accumulator_;                                                 //vector3
  DirectX::XMVECTOR angular_constraints_ = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f); //vector3

  //mass data
  MassData mass_data_;
  DirectX::XMVECTOR global_centroid_;
  //vector3 - physics calculation with center of mass.
  DirectX::XMMATRIX global_inertia_ = DirectX::XMMatrixIdentity();         //matrix3x3
  DirectX::XMMATRIX global_inverse_inertia_ = DirectX::XMMatrixIdentity(); //matrix3x3

  //tool
  const DirectX::XMFLOAT3 zero_vector_ = DirectX::XMFLOAT3();
};
} // namespace Octane
