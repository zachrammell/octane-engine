#include <OctaneEngine/Physics/Ellipsoid.h>

namespace Octane
{
Ellipsoid::Ellipsoid() : radius_() {}

Ellipsoid::~Ellipsoid() {}

DirectX::XMVECTOR Ellipsoid::Support(const DirectX::XMVECTOR& direction)
{
  DirectX::XMVECTOR radius = radius_;
  DirectX::XMVECTOR result = HadamardProduct(radius, radius);
  result = HadamardProduct(result, direction);
  result
    = DirectX::XMVectorScale(result, 1.0f / DirectX::XMVector3Length(HadamardProduct(radius, direction)).m128_f32[0]);
  return result;
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
