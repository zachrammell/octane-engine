#pragma once

#include <EASTL/vector.h>
#include <OctaneEngine/Math.h>
#include <OctaneEngine/Physics/Constraints.h>
#include <OctaneEngine/Physics/Primitive.h>
#include <OctaneEngine/Physics/RigidBody.h>

namespace Octane
{
class ContactPoint
{
public:
  ContactPoint();
  ~ContactPoint();
  ContactPoint& operator=(const ContactPoint& rhs);
  bool operator==(const ContactPoint& rhs) const;
  void Swap();
  void Clear();
  ContactPoint SwappedContactPoint() const;

public:
  DirectX::XMVECTOR global_position_a;
  DirectX::XMVECTOR global_position_b;

  DirectX::XMVECTOR local_position_a;
  DirectX::XMVECTOR local_position_b;

  DirectX::XMVECTOR normal;
  DirectX::XMVECTOR r_a;
  DirectX::XMVECTOR r_b;

  float depth = 0.0f;

  //lagrangian multiplier sum.
  float normal_lambda = 0.0f;
  float tangent_lambda = 0.0f;
  float bitangent_lambda = 0.0f;

  Primitive* collider_a = nullptr;
  Primitive* collider_b = nullptr;

  bool is_valid = true;
  bool b_persistent = false;
};

class ContactManifold
{
public:
public:
  ContactManifold() = delete;
  explicit ContactManifold(Primitive* a, Primitive* b);
  ~ContactManifold();
  ContactManifold(const ContactManifold& rhs);
  ContactManifold& operator=(const ContactManifold& rhs);

  void Set(const ContactManifold& manifold);
  void SetPersistentThreshold(float threshold);
  void UpdateInvalidContact();
  void UpdateCurrentManifold(const ContactPoint& new_contact);
  void CutDownManifold();
  size_t ContactsCount() const;
  void ClearContacts();

private:
  float DistanceFromPoint(const ContactPoint& contact, ContactPoint* p0);
  float DistanceFromLineSegment(const ContactPoint& contact, ContactPoint* p0, ContactPoint* p1);
  float DistanceFromTriangle(const ContactPoint& contact, ContactPoint* p0, ContactPoint* p1, ContactPoint* p2);
  bool OnTriangle(ContactPoint* point, ContactPoint* p0, ContactPoint* p1, ContactPoint* p2);

private:
  friend class Resolution;
  friend class NarrowPhase;
  friend class ContactConstraints;

private:
  float persistent_threshold_squared = 0.01f;
  bool is_collide = false;
  Primitive* collider_a = nullptr;
  Primitive* collider_b = nullptr;
  eastl::vector<ContactPoint> contacts;
};

class ContactConstraints final : public Constraints
{
private:
  class Jacobian
  {
  public:
    DirectX::XMVECTOR v_a;
    DirectX::XMVECTOR w_a;
    DirectX::XMVECTOR v_b;
    DirectX::XMVECTOR w_b;
    float bias = 0.0f;
    float effective_mass = 0.0f;
    float total_lambda = 0.0f;
  };

  class VelocityTerm
  {
  public:
    DirectX::XMVECTOR v_a;
    DirectX::XMVECTOR w_a;
    DirectX::XMVECTOR v_b;
    DirectX::XMVECTOR w_b;
  };

  class MassTerm
  {
  public:
    float m_a = 0.0f;
    float m_b = 0.0f;
    DirectX::XMMATRIX i_a;
    DirectX::XMMATRIX i_b;
  };

public:
  explicit ContactConstraints(ContactManifold* input, float restitution, float friction, float tangent_speed = 0.0f);
  ~ContactConstraints();

  void Release() override;
  void Generate(float dt) override;
  void Solve(float dt) override;
  void Apply() override;

private:
  void GenerateBasis(const DirectX::XMVECTOR& normal, DirectX::XMVECTOR& tangent, DirectX::XMVECTOR& bitangent);
  void WarmStart();
  void InitializeJacobian(
    const ContactPoint& contact,
    const DirectX::XMVECTOR& direction,
    Jacobian& jacobian,
    float dt,
    bool b_normal = false) const;
  void SolveJacobian(Jacobian& jacobian, size_t i, bool b_normal = false);

private:
  RigidBody* body_a = nullptr;
  RigidBody* body_b = nullptr;

  ContactManifold* m_manifold = nullptr;
  RigidBody* m_body_a = nullptr;
  RigidBody* m_body_b = nullptr;

  VelocityTerm m_velocity_term;
  MassTerm m_mass_term;
  Jacobian m_normal[4];
  Jacobian m_tangent[4];
  Jacobian m_bitangent[4];
  float m_restitution = 0.0f;
  float m_friction = 1.0f;
  float m_tangent_speed = 0.0f;
  float m_beta = 0.25f;
  float m_slop = 0.05f;
  size_t m_count = 0;
};
} // namespace Octane
