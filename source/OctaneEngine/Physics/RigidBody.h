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

  void SetPosition(const DirectX::XMFLOAT3& position);
  void SetOrientation(const DirectX::XMFLOAT4& orientation);

  void SetStatic();
  bool IsDynamic() const;

  DirectX::XMFLOAT3 GetPosition() const;
  DirectX::XMFLOAT4 GetOrientation() const;


  DirectX::XMVECTOR LocalToWorldPoint(const DirectX::XMVECTOR& local_point) const;
  DirectX::XMVECTOR WorldToLocalPoint(const DirectX::XMVECTOR& world_point) const;
  DirectX::XMVECTOR LocalToWorldVector(const DirectX::XMVECTOR& local_vector) const;
  DirectX::XMVECTOR WorldToLocalVector(const DirectX::XMVECTOR& world_vector) const;

  float Mass() const;
  DirectX::XMMATRIX Inertia() const;
  DirectX::XMVECTOR Centroid() const;
  DirectX::XMVECTOR GetLinearVelocity() const;
  DirectX::XMVECTOR GetAngularVelocity() const;
  void SetLinearVelocity(const DirectX::XMVECTOR& linear_velocity);
  void SetAngularVelocity(const DirectX::XMVECTOR& angular_velocity);

private:
  void SyncToPosition(DirectX::XMFLOAT3& position) const;
  void SyncToOrientation(DirectX::XMFLOAT4& orientation) const;

  private:
  friend class PhysicsSys;

  bool is_dynamic_;
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
};
} // namespace Octane
