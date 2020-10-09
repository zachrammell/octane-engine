#pragma once

#include <OctaneEngine/Math.h>
#include <OctaneEngine/Physics/Constraints.h>
#include <OctaneEngine/Physics/RigidBody.h>

namespace Octane
{

class ContactPoint
{
public:
  ContactPoint();
  ~ContactPoint();

private:
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
