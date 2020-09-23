#include <OctaneEngine/Physics/Ellipsoid.h>

namespace Octane
{
Ellipsoid::Ellipsoid() : radius_() {}

Ellipsoid::~Ellipsoid() {}

DirectX::XMVECTOR Ellipsoid::Support(const DirectX::XMVECTOR& direction)
{
  return direction;
}

DirectX::XMVECTOR Ellipsoid::GetNormal(const DirectX::XMVECTOR& local_point)
{
  return local_point;
}

float Ellipsoid::GetVolume()
{
  return 0.0f;
}
} // namespace Octane
