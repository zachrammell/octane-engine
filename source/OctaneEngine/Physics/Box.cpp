#include <OctaneEngine/Physics/Box.h>

namespace Octane
{
Box::Box()
  : vertices_{}
{
}

Box::~Box()
{
}

DirectX::XMVECTOR Box::Support(const DirectX::XMVECTOR& direction)
{
  float p = -FLT_MAX;
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
}
