#include <OctaneEngine/Physics/Capsule.h>

namespace Octane
{
Capsule::Capsule() : radius_() {}

Capsule::~Capsule() {}

DirectX::XMVECTOR Capsule::Support(const DirectX::XMVECTOR& direction) {}

DirectX::XMVECTOR Capsule::GetNormal(const DirectX::XMVECTOR& local_point) {}

float Capsule::GetVolume() {}
} // namespace Octane
