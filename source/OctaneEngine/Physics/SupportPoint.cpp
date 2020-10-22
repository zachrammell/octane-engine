#include <OctaneEngine/Physics/SupportPoint.h>
namespace Octane
{
SupportPoint::SupportPoint() : global(), local_a(), local_b(), index(0) {}

SupportPoint::SupportPoint(
  const DirectX::XMVECTOR& globalvec,
  const DirectX::XMVECTOR& local1,
  const DirectX::XMVECTOR& local2,
  size_t idx)
  : index(idx)
{
  DirectX::XMStoreFloat3(&global, globalvec);
  DirectX::XMStoreFloat3(&local_a, local1);
  DirectX::XMStoreFloat3(&local_b, local2);
}

SupportPoint::SupportPoint(
  const DirectX::XMFLOAT3& globalvec,
  const DirectX::XMFLOAT3& local1,
  const DirectX::XMFLOAT3& local2,
  size_t idx)
  : global(globalvec),
    local_a(local1),
    local_b(local2),
    index(idx)
{
}

SupportPoint::SupportPoint(const SupportPoint& rhs)
  : global(rhs.global),
    local_a(rhs.local_a),
    local_b(rhs.local_b),
    index(rhs.index)
{
}

SupportPoint::~SupportPoint() {}

SupportPoint& SupportPoint::operator=(const SupportPoint& rhs)
{
  if (this != &rhs)
  {
    global = rhs.global;
    local_a = rhs.local_a;
    local_b = rhs.local_b;
    index = rhs.index;
  }
  return *this;
}

DirectX::XMFLOAT3& SupportPoint::operator[](size_t i)
{
  return (&global)[i];
}

DirectX::XMFLOAT3 const& SupportPoint::operator[](size_t i) const
{
  return (&global)[i];
}

bool SupportPoint::operator==(const SupportPoint& rhs)
{
  return global.x == rhs.global.x && global.y == rhs.global.y && global.z == rhs.global.z;
}

bool SupportPoint::operator==(const SupportPoint& rhs) const
{
  return global.x == rhs.global.x && global.y == rhs.global.y && global.z == rhs.global.z;
}

bool SupportPoint::IsValid() const
{
  return Math::IsValid(global.x) && Math::IsValid(global.y) && Math::IsValid(global.z);
}
} // namespace Octane