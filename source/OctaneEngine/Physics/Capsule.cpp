#include <OctaneEngine/Physics/Capsule.h>

namespace Octane
{
Capsule::Capsule() : radius_() {}

Capsule::~Capsule() {}

DirectX::XMVECTOR Capsule::Support(const DirectX::XMVECTOR& direction)
{
  DirectX::XMVECTOR axis_vector = DirectX::XMVectorSet(0.0f, height_ * 0.5f, 0.0f, 0.0f);
  DirectX::XMVECTOR radius = radius_;
  DirectX::XMVECTOR result = HadamardProduct(radius, radius);
  result = HadamardProduct(result, direction);
  float denominator = DirectX::XMVector3Length(HadamardProduct(radius, direction)).m128_f32[0];
  result
    = denominator == 0.0f ? DirectX::XMVectorScale(result, 0.0f) : DirectX::XMVectorScale(result, 1.0f / denominator);
  float top = DirectX::XMVector3Dot(direction, axis_vector).m128_f32[0];
  float bot = DirectX::XMVector3Dot(direction, DirectX::XMVectorNegate(axis_vector)).m128_f32[0];
  if (top > bot)
  {
    result = DirectX::XMVectorAdd(result, axis_vector);
  }
  else
  {
    result = DirectX::XMVectorSubtract(result, axis_vector);
  }
  return result;
}

DirectX::XMVECTOR Capsule::GetNormal(const DirectX::XMVECTOR& local_point)
{
  return local_point;
}

float Capsule::GetVolume()
{
  return Math::PI * DirectX::XMVectorGetX(radius_) * DirectX::XMVectorGetZ(radius_)
         * (DirectX::XMVectorGetY(radius_) * 4.0f / 3.0f + height_);
}

void Capsule::CalculateMassData(float density)
{
  //calculate mass of density
  mass_ = density * GetVolume();

  //a, b is x-z plane's param
  float a = DirectX::XMVectorGetX(radius_);
  float b = DirectX::XMVectorGetZ(radius_);

  //c, h is y axis param
  float c = DirectX::XMVectorGetY(radius_); // height of radius
  float h = height_;

  //calculate local inertia tensor
  float multi_a = 2.0f * c * mass_ / (4.0f * c + 3.0f * h);
  float multi_b = 3.0f * h * mass_ / (4.0f * c + 3.0f * h);
  float it_xx
    = multi_a * (0.4f * (b * b + c * c) + 0.75f * h * c + 0.5f * h * h) + multi_b * (0.25f * b * b + h * h / 12.0f);
  float it_zz
    = multi_a * (0.4f * (a * a + c * c) + 0.75f * h * c + 0.5f * h * h) + multi_b * (0.25f * a * a + h * h / 12.0f);
  float it_yy = multi_a * 0.4f * (a * a + b * b) + multi_b * 0.25f * (a * a + b * b);

  DirectX::XMFLOAT3X3 inertia;
  inertia._11 = it_xx;
  inertia._22 = it_yy;
  inertia._33 = it_zz;
  local_inertia_ = DirectX::XMLoadFloat3x3(&inertia);

  //calculate center of mass
  local_centroid_ = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}
} // namespace Octane
