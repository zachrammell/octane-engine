#include <OctaneEngine/Physics/SupportPoint.h>
namespace Octane
{
SupportPoint::SupportPoint() : global(), local_a(), local_b(), index(0) {}

SupportPoint::SupportPoint(
  const DirectX::XMVECTOR& global,
  const DirectX::XMVECTOR& local1,
  const DirectX::XMVECTOR& local2,
  size_t idx)
  : global(global),
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

DirectX::XMVECTOR& SupportPoint::operator[](size_t index)
{
  return (&global)[index];
}

DirectX::XMVECTOR SupportPoint::operator[](size_t index) const
{
  return (&global)[index];
}

bool SupportPoint::operator==(const SupportPoint& rhs)
{
  return Math::IsEqualVector3(global, rhs.global);
}

bool SupportPoint::operator==(const SupportPoint& rhs) const
{
  return Math::IsEqualVector3(global, rhs.global);
}

bool SupportPoint::IsValid() const
{
  return Math::IsValid(global);
}
} // namespace Octane