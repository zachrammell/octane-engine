#pragma once
#include <DirectXMath.h>

namespace Octane
{

struct MassData
{
  //mass
  float mass         = 1.0f;
  float inverse_mass = 1.0f;
  float pad0 = 0.0f;
  float pad1 = 0.0f;

  //center of mass
  DirectX::XMVECTOR local_centroid = DirectX::XMVECTOR(); //vector3

  //inertia
  DirectX::XMMATRIX local_inertia = DirectX::XMMatrixIdentity();         //matrix3x3
  DirectX::XMMATRIX local_inverse_inertia = DirectX::XMMatrixIdentity(); //matrix3x3
};

} // namespace Octane
