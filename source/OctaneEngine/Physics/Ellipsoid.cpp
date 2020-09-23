#include <OctaneEngine/Physics/Ellipsoid.h>

namespace Octane
{
Ellipsoid::Ellipsoid() : radius_() {}

Ellipsoid::~Ellipsoid() {}

DirectX::XMVECTOR Ellipsoid::Support(const DirectX::XMVECTOR& direction) {}

DirectX::XMVECTOR Ellipsoid::GetNormal(const DirectX::XMVECTOR& local_point) {}

float Ellipsoid::GetVolume() {}
} // namespace Octane
