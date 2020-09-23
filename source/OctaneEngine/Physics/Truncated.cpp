#include <OctaneEngine/Physics/Truncated.h>

namespace Octane
{
Truncated::Truncated() : radius_() {}

Truncated::~Truncated() {}

DirectX::XMVECTOR Truncated::Support(const DirectX::XMVECTOR& direction) {}

DirectX::XMVECTOR Truncated::GetNormal(const DirectX::XMVECTOR& local_point) {}

float Truncated::GetVolume() {}
} // namespace Octane
