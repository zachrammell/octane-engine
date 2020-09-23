#include <OctaneEngine/Physics/Truncated.h>

namespace Octane
{
Truncated::Truncated() : radius_() {}

Truncated::~Truncated() {}

DirectX::XMVECTOR Truncated::Support(const DirectX::XMVECTOR& direction)
{
  DirectX::XMVECTOR axis_vector = DirectX::XMVectorSet(0.0f, height_ * 0.5f, 0.0f, 0.0f);
  DirectX::XMVECTOR result;
  DirectX::XMVECTOR ellipse_dir = DirectX::XMVector3Normalize(DirectX::XMVectorSetY(direction, 0.0f));
  DirectX::XMVECTOR ellipse_radius
    = DirectX::XMVectorSet(DirectX::XMVectorGetX(radius_), 0.0f, DirectX::XMVectorGetY(radius_), 0.0f);
  DirectX::XMVECTOR ellipse_vector = HadamardProduct(ellipse_radius, ellipse_radius);
  ellipse_vector = HadamardProduct(ellipse_vector, ellipse_dir);
  float denominator = DirectX::XMVector3Length(HadamardProduct(ellipse_radius, ellipse_dir)).m128_f32[0];
  ellipse_vector = denominator == 0.0f ? DirectX::XMVectorScale(ellipse_vector, 0.0f)
                                       : DirectX::XMVectorScale(ellipse_vector, 1.0f / denominator);

  float top_support = DirectX::XMVector3Dot(
                        direction,
                        DirectX::XMVectorAdd(DirectX::XMVectorScale(ellipse_vector, ratio_), axis_vector))
                        .m128_f32[0];
  float bot_support
    = DirectX::XMVector3Dot(direction, DirectX::XMVectorSubtract(ellipse_vector, axis_vector)).m128_f32[0];
  if (top_support > bot_support)
  {
    result = DirectX::XMVectorAdd(DirectX::XMVectorScale(ellipse_vector, ratio_), axis_vector);
  }
  else
  {
    result = DirectX::XMVectorSubtract(ellipse_vector, axis_vector);
  }
  return result;
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
