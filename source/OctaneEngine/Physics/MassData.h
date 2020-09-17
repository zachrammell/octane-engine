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
  DirectX::XMFLOAT3 local_centroid;

  //inertia
  DirectX::XMFLOAT3X3 local_inertia;
  DirectX::XMFLOAT3X3 local_inverse_inertia;
};

}
