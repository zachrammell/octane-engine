#include <OctaneEngine/FramerateController.h>
#include <OctaneEngine/Physics/Box.h>
#include <OctaneEngine/Physics/Capsule.h>
#include <OctaneEngine/Physics/Ellipsoid.h>
#include <OctaneEngine/Physics/Truncated.h>
#include <OctaneEngine/Physics/World.h>
#include <OctaneEngine/SystemOrder.h>
#include <iostream>

namespace Octane
{
World::World(Engine* engine) : ISystem(engine) {}

void World::LevelStart() {}

void World::Update()
{
  float dt = 1.0 / 60.0f;
  //Three stage of physics pipeline

  //[Broad Phase]
  //broad_phase->Update(dt);
  //broad_phase->ComputePairs(pairs);

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
  //narrow_phase->GenerateContact(pairs, manifold_table);

  for (auto& pair : potential_pairs_)
  {
    Simplex simplex;
    if (narrow_phase_.GJKCollisionDetection(pair.a, pair.b, simplex))
    {
      std::cout << "Collision!! \n";
      //Do EPA contact generation
      //Send Collision Event or Save Collision State
    }
    else
    {
      std::cout << "None!! \n";
    }
  }

  //[Resolution Phase]
  //resolution_phase->SolveConstraints(manifold_table, &rigid_bodies_, dt);
  {
    //Calculate Forces
    //Solve Velocity Constraints
    //Integrate
    for (auto& body : rigid_bodies_)
    {
      body->Integrate(dt);
      body->UpdateOrientation();
      body->UpdateInertia();
      body->UpdatePosition();
    }
    //Solve Position Constraints
  }
}

void World::LevelEnd()
{
  //TODO clear world!
  //rigid_bodies_.clear();
  //
}

SystemOrder World::GetOrder()
{
  return SystemOrder::World;
}

RigidBody* World::GetRigidBody(size_t index)
{
  return rigid_bodies_[index];
}

RigidBody* World::AddRigidBody()
{
  RigidBody* body = new RigidBody();

  rigid_bodies_.push_back(body);
  //initialize general rigid body here when it need.
  return body;
}

Primitive* World::AddPrimitive(RigidBody* owner, ePrimitiveType type)
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