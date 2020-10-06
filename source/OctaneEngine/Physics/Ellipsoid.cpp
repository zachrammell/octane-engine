#include <OctaneEngine/Physics/Ellipsoid.h>

namespace Octane
{
Ellipsoid::Ellipsoid() : radius_() {}

Ellipsoid::~Ellipsoid() {}

DirectX::XMVECTOR Ellipsoid::Support(const DirectX::XMVECTOR& direction)
{
  DirectX::XMVECTOR radius = radius_;
  DirectX::XMVECTOR result = Math::HadamardProduct(radius, radius);
  result = Math::HadamardProduct(result, direction);
  result
    = DirectX::XMVectorScale(result, 1.0f / DirectX::XMVector3Length(Math::HadamardProduct(radius, direction)).m128_f32[0]);
  return result;
}

DirectX::XMVECTOR Ellipsoid::GetNormal(const DirectX::XMVECTOR& local_point)
{
  return local_point;
}

float Ellipsoid::GetVolume()
{
  return 4.0f / 3.0f * Math::PI * DirectX::XMVectorGetX(radius_) * DirectX::XMVectorGetY(radius_)
         * DirectX::XMVectorGetZ(radius_);
}

void Ellipsoid::CalculateMassData(float density)
{
  //calculate mass of density
  mass_ = density * GetVolume();

  //a, b is x-z plane's param
  float a = DirectX::XMVectorGetX(radius_);
  float b = DirectX::XMVectorGetZ(radius_);
  float c = DirectX::XMVectorGetY(radius_);

  float it_xx = 0.2f * mass_ * (b * b + c * c);
  float it_yy = 0.2f * mass_ * (a * a + b * b);
  float it_zz = 0.2f * mass_ * (a * a + c * c);

  //calculate local inertia tensor
  DirectX::XMFLOAT3X3 inertia;
  inertia._11 = it_xx;
  inertia._22 = it_yy;
  inertia._33 = it_zz;
  local_inertia_ = DirectX::XMLoadFloat3x3(&inertia);

  //calculate center of mass
  local_centroid_ = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}
} // namespace Octane
