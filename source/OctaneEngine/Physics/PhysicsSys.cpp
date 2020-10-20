#include <OctaneEngine/Physics/PhysicsSys.h>

#include <iostream>

#include <OctaneEngine/SystemOrder.h>
#include <OctaneEngine/FramerateController.h>
#include <OctaneEngine/Physics/Box.h>
#include <OctaneEngine/Physics/Capsule.h>
#include <OctaneEngine/Physics/Ellipsoid.h>
#include <OctaneEngine/Physics/Truncated.h>
#include <OctaneEngine/Physics/Simplex.h>

#include <OctaneEngine/Trace.h>

namespace Octane
{
PhysicsSys::PhysicsSys(Engine* engine) : ISystem(engine) {}

void PhysicsSys::LevelStart() {}

void PhysicsSys::Update()
{
  float dt = 1.0f / 60.0f;
  //Three stage of physics pipeline

  //[Broad Phase]
  //broad_phase->Update(dt);
  //broad_phase->ComputePairs(pairs);

  //N^2 Brute Force Broad Phase
  potential_pairs_.clear();
  auto primitive_end = primitives_.end();
  for (auto it = primitives_.begin(); it != primitive_end; ++it)
  {
    auto jt_begin = it;
    for (auto jt = ++jt_begin; jt != primitive_end; ++jt)
    {
      Primitive* collider_a = (*it);
      Primitive* collider_b = (*jt);

      potential_pairs_.emplace_back(collider_a, collider_b);
    }
  }

  //[Narrow Phase]
  narrow_phase_.GenerateContact(potential_pairs_, &manifold_table_);

  //[Resolution Phase]
  resolution_phase_.Solve(&manifold_table_, &rigid_bodies_, dt);
 }

void PhysicsSys::LevelEnd()
{
  //TODO clear world!
  //rigid_bodies_.clear();
  //
}

SystemOrder PhysicsSys::GetOrder()
{
  return SystemOrder::World;
}

RigidBody* PhysicsSys::GetRigidBody(size_t index)
{
  return rigid_bodies_[index];
}

RigidBody* PhysicsSys::AddRigidBody()
{
  RigidBody* body = new RigidBody();

  rigid_bodies_.push_back(body);
  //initialize general rigid body here when it need.
  return body;
}

Primitive* PhysicsSys::AddPrimitive(RigidBody* owner, ePrimitiveType type)
{
  Primitive* primitive = nullptr;

  switch (type)
  {
  case ePrimitiveType::Box: primitive = new Box(); break;
  case ePrimitiveType::Capsule: primitive = new Capsule(); break;
  case ePrimitiveType::Ellipsoid: primitive = new Ellipsoid(); break;
  case ePrimitiveType::Truncated: primitive = new Truncated(); break;
  }

  if (primitive != nullptr)
  {
    primitive->rigid_body_ = owner;
    primitives_.push_back(primitive);
  }

  return primitive;
}
} // namespace Octane
