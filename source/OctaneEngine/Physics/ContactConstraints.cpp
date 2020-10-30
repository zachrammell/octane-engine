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
    this->b_persistent = rhs.b_persistent;
  }
  return *this;
}

bool ContactPoint::operator==(const ContactPoint& rhs) const
{
  return Math::IsEqualVector3(normal, rhs.normal) && depth == rhs.depth
         && Math::IsEqualVector3(local_position_a, rhs.local_position_a)
         && Math::IsEqualVector3(local_position_b, rhs.local_position_b);
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
  b_persistent = false;
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
  result.b_persistent = this->b_persistent;
  return result;
}

ContactManifold::ContactManifold(Primitive* a, Primitive* b)
  : collider_a(a),
    collider_b(b)
{
}

ContactManifold::~ContactManifold()
{
}

ContactManifold::ContactManifold(const ContactManifold& rhs)
{
  collider_a = rhs.collider_a;
  collider_b = rhs.collider_b;
  persistent_threshold_squared = rhs.persistent_threshold_squared;
  is_collide = rhs.is_collide;
  contacts = rhs.contacts;
}

ContactManifold& ContactManifold::operator=(const ContactManifold& rhs)
{
  if (this != &rhs)
  {
    collider_a = rhs.collider_a;
    collider_b = rhs.collider_b;
    persistent_threshold_squared = rhs.persistent_threshold_squared;
    is_collide = rhs.is_collide;
    contacts = rhs.contacts;
  }
  return *this;
}

void ContactManifold::Set(const ContactManifold& manifold)
{
  collider_a = manifold.collider_a;
  collider_b = manifold.collider_b;
  persistent_threshold_squared = manifold.persistent_threshold_squared;
  is_collide = manifold.is_collide;
  contacts = manifold.contacts;
}

void ContactManifold::SetPersistentThreshold(float threshold)
{
  persistent_threshold_squared = threshold * threshold;
}

void ContactManifold::UpdateInvalidContact()
{
  eastl::vector<ContactPoint*> remove_list;
  for (auto& contact : contacts)
  {
    DirectX::XMVECTOR local_to_global_a
      = contact.collider_a->GetRigidBody()->LocalToWorldPoint(contact.local_position_a);
    DirectX::XMVECTOR local_to_global_b
      = contact.collider_b->GetRigidBody()->LocalToWorldPoint(contact.local_position_b);
    DirectX::XMVECTOR r_ab = DirectX::XMVectorSubtract(local_to_global_b, local_to_global_a);
    DirectX::XMVECTOR r_a = DirectX::XMVectorSubtract(contact.global_position_a, local_to_global_a);
    DirectX::XMVECTOR r_b = DirectX::XMVectorSubtract(contact.global_position_b, local_to_global_b);
    bool b_still_penetrating = Math::DotProductVector3(contact.normal, r_ab) <= 0.0f;
    bool b_r_a_close_enough = DirectX::XMVector3LengthSq(r_a).m128_f32[0] < persistent_threshold_squared;
    bool b_r_b_close_enough = DirectX::XMVector3LengthSq(r_b).m128_f32[0] < persistent_threshold_squared;
    if (b_r_a_close_enough && b_r_b_close_enough && b_still_penetrating)
    {
      contact.b_persistent = true;
    }
    else
    {
      remove_list.push_back(&contact);
    }
  }
  for (auto it = remove_list.begin(); it != remove_list.end(); ++it)
  {
    for (auto itc = contacts.begin(); itc != contacts.end();)
    {
      if (*itc == **it)
      {
        contacts.erase(itc++);
        break;
      }
      ++itc;
    }
  }
  remove_list.clear();
}

void ContactManifold::UpdateCurrentManifold(const ContactPoint& new_contact)
{
  bool add_contact = false;
  for (auto& contact : contacts)
  {
    DirectX::XMVECTOR r_a = DirectX::XMVectorSubtract(new_contact.global_position_a, contact.global_position_a);
    DirectX::XMVECTOR r_b = DirectX::XMVectorSubtract(new_contact.global_position_b, contact.global_position_b);
    bool r_a_far_enough = DirectX::XMVector3LengthSq(r_a).m128_f32[0] > persistent_threshold_squared;
    bool r_b_far_enough = DirectX::XMVector3LengthSq(r_b).m128_f32[0] > persistent_threshold_squared;

    // proximity check
    if (r_a_far_enough && r_b_far_enough)
    {
      //add new_contact
      add_contact = true;
    }
  }
  if (add_contact == true || contacts.empty())
  {
    if (new_contact.collider_a == collider_b && new_contact.collider_b == collider_a)
    {
      contacts.push_back(new_contact.SwappedContactPoint());
    }
    else
    {
      contacts.push_back(new_contact);
    }
  }
}

void ContactManifold::CutDownManifold()
{
  if (contacts.size() < 4)
  {
    return;
  }
  // find the deepest penetrating one
  ContactPoint* deepest = nullptr;
  float penetration = Math::REAL_NEGATIVE_MAX;
  for (auto& contact : contacts)
  {
    if (contact.depth > penetration)
    {
      penetration = contact.depth;
      deepest = &contact;
    }
  }
  // find second contact
  ContactPoint* furthest1 = nullptr;
  float distance_squared1 = Math::REAL_NEGATIVE_MAX;
  for (auto& contact : contacts)
  {
    float dist = DistanceFromPoint(contact, deepest);
    if (dist > distance_squared1)
    {
      distance_squared1 = dist;
      furthest1 = &contact;
    }
  }
  // find third contact
  ContactPoint* furthest2 = nullptr;
  float distance_squared2 = Math::REAL_NEGATIVE_MAX;
  for (auto& contact : contacts)
  {
    float dist = DistanceFromLineSegment(contact, deepest, furthest1);
    if (dist > distance_squared2)
    {
      distance_squared2 = dist;
      furthest2 = &contact;
    }
  }
  // find fourth contact
  ContactPoint* furthest3 = nullptr;
  float distance_squared3 = Math::REAL_NEGATIVE_MAX;
  for (auto& contact : contacts)
  {
    float dist = DistanceFromTriangle(contact, deepest, furthest1, furthest2);
    if (dist > distance_squared3)
    {
      distance_squared3 = dist;
      furthest3 = &contact;
    }
  }
  contacts.clear();
  if (deepest != nullptr)
  {
    contacts.push_back(*deepest);
  }
  if (furthest1 != nullptr)
  {
    contacts.push_back(*furthest1);
  }
  if (furthest2 != nullptr)
  {
    contacts.push_back(*furthest2);
  }
  bool on_triangle = OnTriangle(furthest3, deepest, furthest1, furthest2);
  if (on_triangle == false && furthest3 != nullptr)
  {
    contacts.push_back(*furthest3);
  }
}

size_t ContactManifold::ContactsCount() const
{
  return contacts.size();
}

void ContactManifold::ClearContacts()
{
  contacts.clear();
  is_collide = false;
}

float ContactManifold::DistanceFromPoint(const ContactPoint& contact, ContactPoint* p0)
{
  if (p0 == nullptr)
  {
    return Math::REAL_NEGATIVE_MAX;
  }
  return DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(contact.global_position_a, p0->global_position_a))
    .m128_f32[0];
}

float ContactManifold::DistanceFromLineSegment(const ContactPoint& contact, ContactPoint* p0, ContactPoint* p1)
{
  if (p0 == nullptr || p1 == nullptr)
  {
    return Math::REAL_NEGATIVE_MAX;
  }
  DirectX::XMVECTOR n = DirectX::XMVectorSubtract(p1->global_position_a, p0->global_position_a);
  DirectX::XMVECTOR pa = DirectX::XMVectorSubtract(p0->global_position_a, contact.global_position_a);
  DirectX::XMVECTOR c = DirectX::XMVectorScale(n, Math::DotProductVector3(pa, n) / Math::DotProductVector3(n, n));
  DirectX::XMVECTOR d = DirectX::XMVectorSubtract(pa, c);
  return Math::DotProductVector3(d, d);
}

float ContactManifold::DistanceFromTriangle(
  const ContactPoint& contact,
  ContactPoint* p0,
  ContactPoint* p1,
  ContactPoint* p2)
{
  if (p0 == nullptr || p1 == nullptr || p2 == nullptr)
  {
    return Math::REAL_NEGATIVE_MAX;
  }
  DirectX::XMVECTOR v0 = DirectX::XMVectorSubtract(p1->global_position_a, p0->global_position_a);
  DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(p2->global_position_a, p0->global_position_a);
  DirectX::XMVECTOR v2 = DirectX::XMVectorSubtract(contact.global_position_a, p0->global_position_a);
  float d00 = Math::DotProductVector3(v0, v0);
  float d01 = Math::DotProductVector3(v0, v1);
  float d11 = Math::DotProductVector3(v1, v1);
  float d20 = Math::DotProductVector3(v2, v0);
  float d21 = Math::DotProductVector3(v2, v1);
  float denominator = d00 * d11 - d01 * d01;
  float v = (d11 * d20 - d01 * d21) / denominator;
  float w = (d00 * d21 - d01 * d20) / denominator;
  float u = 1.0f - v - w;
  DirectX::XMVECTOR closest = DirectX::XMVectorAdd(
    DirectX::XMVectorAdd(
      DirectX::XMVectorScale(p0->global_position_a, u),
      DirectX::XMVectorScale(p1->global_position_a, v)),
    DirectX::XMVectorScale(p2->global_position_a, w));
  return DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(closest, contact.global_position_a)).m128_f32[0];
}

bool ContactManifold::OnTriangle(ContactPoint* point, ContactPoint* p0, ContactPoint* p1, ContactPoint* p2)
{
  if (point == nullptr || p0 == nullptr || p1 == nullptr || p2 == nullptr)
  {
    return false;
  }
  DirectX::XMVECTOR edge1 = DirectX::XMVectorSubtract(p1->global_position_a, p0->global_position_a);
  DirectX::XMVECTOR edge2 = DirectX::XMVectorSubtract(p2->global_position_a, p1->global_position_a);
  DirectX::XMVECTOR normal = DirectX::XMVector3Cross(edge1, edge2);
  DirectX::XMVECTOR w_test
    = DirectX::XMVector3Cross(edge1, DirectX::XMVectorSubtract(point->global_position_a, p0->global_position_a));
  if (Math::DotProductVector3(w_test, normal) < 0.0f)
  {
    return false;
  }
  w_test = DirectX::XMVector3Cross(edge2, DirectX::XMVectorSubtract(point->global_position_a, p1->global_position_a));
  if (Math::DotProductVector3(w_test, normal) < 0.0f)
  {
    return false;
  }
  DirectX::XMVECTOR edge3 = DirectX::XMVectorSubtract(p0->global_position_a, p2->global_position_a);
  w_test = DirectX::XMVector3Cross(edge3, DirectX::XMVectorSubtract(point->global_position_a, p2->global_position_a));
  if (Math::DotProductVector3(w_test, normal) < 0.0f)
  {
    return false;
  }
  return true;
}

ContactConstraints::ContactConstraints(ContactManifold* input, float restitution, float friction, float tangent_speed)
  : m_manifold(input),
    m_velocity_term(),
    m_mass_term(),
    m_normal{},
    m_tangent{},
    m_bitangent{},
    m_restitution(restitution),
    m_friction(friction),
    m_tangent_speed(tangent_speed)

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
  m_body_a = m_manifold->collider_a->GetRigidBody();
  m_body_b = m_manifold->collider_b->GetRigidBody();

  if (m_body_a->IsGhost() || m_body_b->IsGhost())
  {
    return;
  }

  //set mass
  m_mass_term.m_a = m_body_a->Mass();
  m_mass_term.i_a = m_body_a->Inertia();
  m_mass_term.m_b = m_body_b->Mass();
  m_mass_term.i_b = m_body_b->Inertia();
  //velocity term
  m_velocity_term.v_a = m_body_a->GetLinearVelocity();
  m_velocity_term.w_a = m_body_a->GetAngularVelocity();
  m_velocity_term.v_b = m_body_b->GetLinearVelocity();
  m_velocity_term.w_b = m_body_b->GetAngularVelocity();
  m_count = m_manifold->contacts.size();

  DirectX::XMVECTOR tangent, bitangent;

  for (size_t i = 0; i < m_count; ++i)
  {
    GenerateBasis(m_manifold->contacts[i].normal, tangent, bitangent);
    m_manifold->contacts[i].r_a
      = DirectX::XMVectorSubtract(m_manifold->contacts[i].global_position_a, m_body_a->Centroid());
    m_manifold->contacts[i].r_b
      = DirectX::XMVectorSubtract(m_manifold->contacts[i].global_position_b, m_body_b->Centroid());
    InitializeJacobian(m_manifold->contacts[i], m_manifold->contacts[i].normal, m_normal[i], dt, true);
    InitializeJacobian(m_manifold->contacts[i], tangent, m_tangent[i], dt);
    InitializeJacobian(m_manifold->contacts[i], bitangent, m_bitangent[i], dt);
  }
}

void ContactConstraints::Solve(float /*dt*/)
{
  if (m_body_a->IsGhost() || m_body_b->IsGhost())
  {
    return;
  }
  for (size_t i = 0; i < m_count; ++i)
  {
    // Solve tangent constraints first because non-penetration is more important than friction.
    SolveJacobian(m_tangent[i], i, false);
    SolveJacobian(m_bitangent[i], i, false);
    // Solve normal constraints
    SolveJacobian(m_normal[i], i, true);
  }
}

void ContactConstraints::Apply()
{
  if (m_body_a->IsGhost() || m_body_b->IsGhost())
  {
    return;
  }
  for (size_t i = 0; i < m_count; ++i)
  {
    m_manifold->contacts[i].tangent_lambda = m_tangent[i].total_lambda;
    m_manifold->contacts[i].bitangent_lambda = m_bitangent[i].total_lambda;
    m_manifold->contacts[i].normal_lambda = m_normal[i].total_lambda;
  }

  m_body_a->SetLinearVelocity(m_velocity_term.v_a);
  m_body_a->SetAngularVelocity(m_velocity_term.w_a);
  m_body_b->SetLinearVelocity(m_velocity_term.v_b);
  m_body_b->SetAngularVelocity(m_velocity_term.w_b);
}

void ContactConstraints::WarmStart()
{
  //do noting now.
}

void ContactConstraints::GenerateBasis(
  const DirectX::XMVECTOR& normal,
  DirectX::XMVECTOR& tangent,
  DirectX::XMVECTOR& bitangent)
{
  float x = DirectX::XMVectorGetX(normal);
  float y = DirectX::XMVectorGetY(normal);
  float z = DirectX::XMVectorGetZ(normal);

  //sqrt(1/3) = 0.57735
  if (fabsf(x) >= 0.57735f)
  {
    tangent = DirectX::XMVectorSet(y, -x, 0.0f, 0.0f);
  }
  else
  {
    tangent = DirectX::XMVectorSet(0.0f, z, -y, 0.0f);
  }
  tangent = DirectX::XMVector3Normalize(tangent);
  bitangent = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(normal, tangent));
}

void ContactConstraints::InitializeJacobian(
  const ContactPoint& contact,
  const DirectX::XMVECTOR& direction,
  Jacobian& jacobian,
  float dt,
  bool b_normal) const
{
  jacobian.v_a = DirectX::XMVectorNegate(direction);
  jacobian.w_a = DirectX::XMVectorNegate(DirectX::XMVector3Cross(contact.r_a, direction));
  jacobian.v_b = direction;
  jacobian.w_b = DirectX::XMVector3Cross(contact.r_b, direction);
  jacobian.bias = 0.0f;
  if (b_normal)
  {
    float beta = m_beta;
    DirectX::XMVECTOR relative_velocity = DirectX::XMVectorSubtract(m_velocity_term.v_b, m_velocity_term.v_a);
    relative_velocity
      = DirectX::XMVectorSubtract(relative_velocity, DirectX::XMVector3Cross(m_velocity_term.w_a, contact.r_a));
    relative_velocity
      = DirectX::XMVectorAdd(relative_velocity, DirectX::XMVector3Cross(m_velocity_term.w_b, contact.r_b));

    float closing_velocity = Math::DotProductVector3(relative_velocity, direction);
    jacobian.bias = -(beta / dt) * std::max(contact.depth - m_slop, 0.0f) + m_restitution * closing_velocity;
  }
  else
  {
    jacobian.bias = -m_tangent_speed;
  }
  float k = m_mass_term.m_a + m_mass_term.m_b
            + Math::DotProductVector3(XMVector3Transform(jacobian.w_a, m_mass_term.i_a), jacobian.w_a)
            + Math::DotProductVector3(XMVector3Transform(jacobian.w_b, m_mass_term.i_b), jacobian.w_b);
  jacobian.effective_mass = k > 0.0f ? 1.0f / k : 0.0f;
  jacobian.total_lambda = 0.0f;
}

void ContactConstraints::SolveJacobian(Jacobian& jacobian, size_t i, bool b_normal)
{
  // jv = Jacobian * velocity vector
  float jv = Math::DotProductVector3(jacobian.v_a, m_velocity_term.v_a)
             + Math::DotProductVector3(jacobian.w_a, m_velocity_term.w_a)
             + Math::DotProductVector3(jacobian.v_b, m_velocity_term.v_b)
             + Math::DotProductVector3(jacobian.w_b, m_velocity_term.w_b);
  // raw lambda
  float lambda = jacobian.effective_mass * -(jv + jacobian.bias);
  // clamped lambda
  float old_total_lambda = jacobian.total_lambda;
  if (b_normal)
  {
    //normal - contact resolution : lambda >= 0
    jacobian.total_lambda = std::max(0.0f, jacobian.total_lambda + lambda);
  }
  else
  {
    //tangent - friction : -max_friction <= lambda <= max_friction
    float max_friction = m_friction * m_normal[i].total_lambda;
    jacobian.total_lambda = std::clamp(jacobian.total_lambda + lambda, -max_friction, max_friction);
  }
  lambda = jacobian.total_lambda - old_total_lambda;
  // velocity correction
  m_velocity_term.v_a
    = DirectX::XMVectorAdd(m_velocity_term.v_a, DirectX::XMVectorScale(jacobian.v_a, m_mass_term.m_a * lambda));
  m_velocity_term.w_a = DirectX::XMVectorAdd(
    m_velocity_term.w_a,
    DirectX::XMVectorScale(XMVector3Transform(jacobian.w_a, m_mass_term.i_a), lambda));
  m_velocity_term.v_b
    = DirectX::XMVectorAdd(m_velocity_term.v_b, DirectX::XMVectorScale(jacobian.v_b, m_mass_term.m_b * lambda));
  m_velocity_term.w_b = DirectX::XMVectorAdd(
    m_velocity_term.w_b,
    DirectX::XMVectorScale(XMVector3Transform(jacobian.w_b, m_mass_term.i_b), lambda));
}
} // namespace Octane
