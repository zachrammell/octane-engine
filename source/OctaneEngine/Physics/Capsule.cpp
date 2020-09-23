#include <OctaneEngine/Physics/Capsule.h>

namespace Octane
{
Capsule::Capsule() : radius_() {}

Capsule::~Capsule() {}

DirectX::XMVECTOR Capsule::Support(const DirectX::XMVECTOR& direction)
{
  return direction;
}

DirectX::XMVECTOR Capsule::GetNormal(const DirectX::XMVECTOR& local_point)
{
  return local_point;
}

float Capsule::GetVolume()
{
  return 0.0f;
}
} // namespace Octane
