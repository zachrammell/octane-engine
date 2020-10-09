#include <OctaneEngine/Physics/ContactConstraints.h>
#include <algorithm>

namespace Octane
{
ContactPoint::ContactPoint()
  : global_position_a(),
    global_position_b(),
    local_position_a(),
    local_position_b(),
    normal(),
    r_a(),
    r_b()
{
}

ContactPoint::~ContactPoint()
{
}

ContactPoint& ContactPoint::operator=(const ContactPoint& rhs)
{
  if (this != &rhs)
  {
    this->collider_a = rhs.collider_a;
    this->collider_b = rhs.collider_b;
    this->global_position_a = rhs.global_position_a;
    this->global_position_b = rhs.global_position_b;
    this->local_position_a = rhs.local_position_a;
    this->local_position_b = rhs.local_position_b;
    this->normal = rhs.normal;
    this->depth = rhs.depth;
    this->normal_lambda = rhs.normal_lambda;
    this->tangent_lambda = rhs.tangent_lambda;
    this->bitangent_lambda = rhs.bitangent_lambda;
    this->is_valid = rhs.is_valid;
  }
  return *this;
}

bool ContactPoint::operator==(const ContactPoint& rhs) const
{
  return (
    Math::IsEqualVector3(normal, rhs.normal) && depth == rhs.depth
    && Math::IsEqualVector3(local_position_a, rhs.local_position_a)
    && Math::IsEqualVector3(local_position_b, rhs.local_position_b));
}

void ContactPoint::Swap()
{
  std::swap(collider_a, collider_b);
  std::swap(global_position_a, global_position_b);
  std::swap(local_position_a, local_position_b);
  normal = DirectX::XMVectorNegate(normal);
}

void ContactPoint::Clear()
{
  global_position_a = DirectX::XMVECTOR();
  global_position_b = DirectX::XMVECTOR();
  local_position_a = DirectX::XMVECTOR();
  local_position_b = DirectX::XMVECTOR();
  normal = DirectX::XMVECTOR();
  depth = 0.0f;
  normal_lambda = 0.0f;
  tangent_lambda = 0.0f;
  bitangent_lambda = 0.0f;
  is_valid = true;
}

ContactPoint ContactPoint::SwappedContactPoint() const
{
  ContactPoint result;
  result.normal = DirectX::XMVectorNegate(normal);
  result.depth = this->depth;
  result.collider_a = this->collider_b;
  result.collider_b = this->collider_a;
  result.global_position_a = this->global_position_b;
  result.global_position_b = this->global_position_a;
  result.local_position_a = this->local_position_b;
  result.local_position_b = this->local_position_a;
  result.is_valid = this->is_valid;
  return result;
}

ContactManifold::ContactManifold()
{
}

ContactManifold::~ContactManifold()
{
}

ContactConstraints::ContactConstraints()
{
}

ContactConstraints::~ContactConstraints()
{
}

void ContactConstraints::Release()
{
}

void ContactConstraints::Generate(float dt)
{
}

void ContactConstraints::Solve(float dt)
{
}

void ContactConstraints::Apply()
{
}
} // namespace Octane
