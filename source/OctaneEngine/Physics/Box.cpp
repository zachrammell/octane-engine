#include <OctaneEngine/Physics/Box.h>

namespace Octane
{
Box::Box() : vertices_ {} {}

Box::~Box() {}

DirectX::XMVECTOR Box::Support(const DirectX::XMVECTOR& direction)
{
  float p = Math::REAL_NEGATIVE_MAX;
  DirectX::XMVECTOR result;
  for (size_t i = 0; i < 8; ++i)
  {
    float projection = DirectX::XMVector3Dot(vertices_[i], direction).m128_f32[0];
    if (projection > p)
    {
      result = vertices_[i];
      p = projection;
    }
  }
  return result;
}

DirectX::XMVECTOR Box::GetNormal(const DirectX::XMVECTOR& local_point)
{
  DirectX::XMVECTOR normal;
  //float w = DirectX::XMVectorSubtract(vertices_[0], vertices_[4]).m128_f32[0];
  //float h = DirectX::XMVectorSubtract(vertices_[0], vertices_[2]).m128_f32[1];
  //float d = DirectX::XMVectorSubtract(vertices_[0], vertices_[1]).m128_f32[2];
  //DirectX::XMVECTOR size = DirectX::XMVectorSet(w, h, d, 0.0f);
  //float min = FLT_MAX;
  //float distance = fabsf(size.x - fabsf(local_point.x));
  //if (distance < min)
  //{
  //  min = distance;
  //  normal.Set(1.0f, 0.0f, 0.0f);
  //  normal *= Math::Signum(local_point.x);
  //}
  //distance = fabsf(size.y - fabsf(local_point.y));
  //if (distance < min)
  //{
  //  min = distance;
  //  normal.Set(0.0f, 1.0f, 0.0f);
  //  normal *= Math::Signum(local_point.y);
  //}
  //distance = fabsf(size.z - fabsf(local_point.z));
  //if (distance < min)
  //{
  //  normal.Set(0.0f, 0.0f, 1.0f);
  //  normal *= Math::Signum(local_point.z);
  //}
  return normal;
}

float Box::GetVolume()
{
  float w = DirectX::XMVectorSubtract(vertices_[0], vertices_[4]).m128_f32[0];
  float h = DirectX::XMVectorSubtract(vertices_[0], vertices_[2]).m128_f32[1];
  float d = DirectX::XMVectorSubtract(vertices_[0], vertices_[1]).m128_f32[2];
  return w * h * d;
}

void Box::CalculateMassData(float density)
{
  float w = DirectX::XMVectorSubtract(vertices_[0], vertices_[4]).m128_f32[0];
  float h = DirectX::XMVectorSubtract(vertices_[0], vertices_[2]).m128_f32[1];
  float d = DirectX::XMVectorSubtract(vertices_[0], vertices_[1]).m128_f32[2];

  //calculate mass of density
  mass_ = density * w * h * d;

  //calculate local inertia tensor
  DirectX::XMFLOAT3X3 inertia;
  inertia._11 = mass_ / 12.0f * (h * h + d * d);
  inertia._22 = mass_ / 12.0f * (w * w + d * d);
  inertia._33 = mass_ / 12.0f * (w * w + h * h);
  local_inertia_ = DirectX::XMLoadFloat3x3(&inertia);

  //calculate center of mass
  local_centroid_ = DirectX::XMVectorSet(
    0.5f * w + DirectX::XMVectorGetX(vertices_[7]),
    0.5f * h + DirectX::XMVectorGetY(vertices_[7]),
    0.5f * d + DirectX::XMVectorGetZ(vertices_[7]),
    0.0f);
}

void Box::SetBox(float width, float height, float depth)
{
  float w = 0.5f * width;
  float h = 0.5f * height;
  float d = 0.5f * depth;
  vertices_[0] = DirectX::XMVectorSet(+w, +h, +d, 0.0f);
  vertices_[1] = DirectX::XMVectorSet(+w, +h, -d, 0.0f);
  vertices_[2] = DirectX::XMVectorSet(+w, -h, +d, 0.0f);
  vertices_[3] = DirectX::XMVectorSet(+w, -h, -d, 0.0f);
  vertices_[4] = DirectX::XMVectorSet(-w, +h, +d, 0.0f);
  vertices_[5] = DirectX::XMVectorSet(-w, +h, -d, 0.0f);
  vertices_[6] = DirectX::XMVectorSet(-w, -h, +d, 0.0f);
  vertices_[7] = DirectX::XMVectorSet(-w, -h, -d, 0.0f);
}
} // namespace Octane
