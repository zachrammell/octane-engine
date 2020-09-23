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
  return 0.0f;
}
} // namespace Octane
