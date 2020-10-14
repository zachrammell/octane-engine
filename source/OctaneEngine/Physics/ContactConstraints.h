#pragma once

#include <OctaneEngine/Math.h>
#include <OctaneEngine/Physics/Constraints.h>
#include <OctaneEngine/Physics/RigidBody.h>
#include <OctaneEngine/Physics/Primitive.h>
#include <EASTL/vector.h>

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
  friend class ContactConstraint;
  friend class ManifoldTable;

private:
  float persistent_threshold_squared = 0.01f;
  bool is_collide = false;
  Primitive* collider_a = nullptr;
  Primitive* collider_b = nullptr;
  eastl::vector<ContactPoint> contacts;
};

class ContactConstraints final : public Constraints
{
public:
  ContactConstraints();
  ~ContactConstraints();

  void Release() override;
  void Generate(float dt) override;
  void Solve(float dt) override;
  void Apply() override;

private:
  RigidBody* body_a = nullptr;
  RigidBody* body_b = nullptr;
};
} // namespace Octane
