#pragma once
#include <DirectXMath.h>

namespace Octane
{

struct MassData
{
  //mass
  float mass         = 1.0f;
  float inverse_mass = 1.0f;

  //center of mass
  DirectX::XMVECTOR local_centroid; //vector3

  //inertia
  DirectX::XMMATRIX local_inertia = DirectX::XMMatrixIdentity();         //matrix3x3
  DirectX::XMMATRIX local_inverse_inertia = DirectX::XMMatrixIdentity(); //matrix3x3
};

} // namespace Octane
