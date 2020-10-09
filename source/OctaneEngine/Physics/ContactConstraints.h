#pragma once

#include <OctaneEngine/Math.h>
#include <OctaneEngine/Physics/Constraints.h>
#include <OctaneEngine/Physics/RigidBody.h>
#include <OctaneEngine/Physics/Primitive.h>

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
};

class ContactManifold
{
public:
  ContactManifold();
  ~ContactManifold();

private:
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
