#include <OctaneEngine/Physics/Truncated.h>

namespace Octane
{
Truncated::Truncated()
{
  type_ = ePrimitiveType::Truncated;
}

Truncated::~Truncated() {}

DirectX::XMVECTOR Truncated::Support(const DirectX::XMVECTOR& direction)
{
  DirectX::XMVECTOR axis_vector = DirectX::XMVectorSet(0.0f, height_ * 0.5f, 0.0f, 0.0f);
  DirectX::XMVECTOR result;
  DirectX::XMVECTOR ellipse_dir = DirectX::XMVector3Normalize(DirectX::XMVectorSetY(direction, 0.0f));
  DirectX::XMVECTOR ellipse_radius
    = DirectX::XMVectorSet(DirectX::XMVectorGetX(radius_), 0.0f, DirectX::XMVectorGetY(radius_), 0.0f);
  DirectX::XMVECTOR ellipse_vector = Math::HadamardProduct(ellipse_radius, ellipse_radius);
  ellipse_vector = Math::HadamardProduct(ellipse_vector, ellipse_dir);
  float denominator = DirectX::XMVector3Length(Math::HadamardProduct(ellipse_radius, ellipse_dir)).m128_f32[0];
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
  float ratio = ratio_ * ratio_ + ratio_ + 1.0f;
  return Math::PI * DirectX::XMVectorGetX(radius_) * DirectX::XMVectorGetY(radius_) * height_ * ratio / 3.0f;
}

void Truncated::CalculateMassData(float density)
{
  //calculate mass of density
  mass_ = density * GetVolume();

  //calculate ratio param
  float ratio = (ratio_ * ratio_ + ratio_ + 1.0f); //r^2 + r + 1
  float ratio_multi_a
    = (ratio_ * ratio_ * ratio_ * ratio_ + ratio_ * ratio_ * ratio_ + ratio_ * ratio_ + ratio_ + 1.0f) / ratio;
  float ratio_multi_b = (ratio_ * ratio_ * ratio_ * ratio_ + 4.0f * ratio_ * ratio_ * ratio_ + 10.0f * ratio_ * ratio_
                         + 4.0f * ratio_ + 1.0f)
                        / (ratio * ratio);

  float a = DirectX::XMVectorGetX(radius_);
  float b = DirectX::XMVectorGetY(radius_);
  float h = height_;

  //calculate local inertia tensor
  float it_xx = 0.15f * mass_ * b * b * ratio_multi_a + 0.0375f * mass_ * h * h * ratio_multi_b;
  float it_zz = 0.15f * mass_ * a * a * ratio_multi_a + 0.0375f * mass_ * h * h * ratio_multi_b;
  float it_yy = 0.15f * mass_ * (a * a + b * b) * ratio_multi_a;
  DirectX::XMFLOAT3X3 inertia;
  inertia._11 = it_xx;
  inertia._22 = it_yy;
  inertia._33 = it_zz;
  local_inertia_ = DirectX::XMLoadFloat3x3(&inertia);

  //calculate center of mass
  local_centroid_ = DirectX::XMVectorSet(
    0.0f,
    ((3.0f * ratio_ * ratio_ + 2.0f * ratio_ + 1.0f) * h / (4.0f * ratio)) - (0.5f * height_),
    0.0f,
    0.0f);
}
} // namespace Octane
