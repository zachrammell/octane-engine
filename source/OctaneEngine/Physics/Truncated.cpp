#include <OctaneEngine/Physics/Truncated.h>

namespace Octane
{
Truncated::Truncated() : radius_() {}

Truncated::~Truncated() {}

DirectX::XMVECTOR Truncated::Support(const DirectX::XMVECTOR& direction)
{
  return direction;
}

DirectX::XMVECTOR Truncated::GetNormal(const DirectX::XMVECTOR& local_point)
{
  return local_point;
}

float Truncated::GetVolume()
{
  return 0.0f;
}
} // namespace Octane
