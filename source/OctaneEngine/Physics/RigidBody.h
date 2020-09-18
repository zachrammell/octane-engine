#pragma once

#include <DirectXMath.h>
#include <OctaneEngine/Physics/MassData.h>

namespace Octane
{

class RigidBody
{
public:
  RigidBody()  = default;
  ~RigidBody() = default;

  void Integrate(float dt);

  void ApplyForce(const DirectX::XMFLOAT3& force, const DirectX::XMFLOAT3& at);
  void ApplyForceCentroid(const DirectX::XMFLOAT3& force);
  void ApplyTorque(const DirectX::XMFLOAT3& torque);

  void UpdateCentroid();
  void UpdatePosition();
  void UpdateInertia();
  void UpdateOrientation();

private:
  //linear data - positional
  DirectX::XMVECTOR position_;           //vector3
  DirectX::XMVECTOR linear_velocity_;    //vector3
  DirectX::XMVECTOR force_accumulator_;  //vector3
  DirectX::XMVECTOR linear_constraints_; //vector3

  //angular data - rotational
  DirectX::XMVECTOR orientation_;         //quaternion
  DirectX::XMVECTOR inverse_orientation_; //quaternion
  DirectX::XMVECTOR angular_velocity_;    //vector3
  DirectX::XMVECTOR torque_accumulator_;  //vector3
  DirectX::XMVECTOR angular_constraints_; //vector3

  //mass data
  MassData mass_data_;
  DirectX::XMVECTOR global_centroid_;        //vector3
  DirectX::XMMATRIX global_inertia_;         //matrix3x3
  DirectX::XMMATRIX global_inverse_inertia_; //matrix3x3

  //tool
  const DirectX::XMFLOAT3 zero_vector_;
};

} // namespace Octane
