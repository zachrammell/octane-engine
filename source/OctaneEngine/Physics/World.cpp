#include <OctaneEngine/FramerateController.h>
#include <OctaneEngine/Physics/World.h>
#include <OctaneEngine/SystemOrder.h>

namespace Octane
{
World::World(Engine* engine) : ISystem(engine) {}

void World::LevelStart() {}

void World::Update()
{
  float dt = 1.0 / 60.0f; //Get<FramerateController>()->GetDeltaTime();
  //Three stage of physics pipeline

  //[Broad Phase]
  //broad_phase->Update(dt);
  //broad_phase->ComputePairs(pairs);

  //[Narrow Phase]
  //narrow_phase->GenerateContact(pairs, manifold_table);
  {
    //Do GJK intersection test
    //Do EPA contact generation
    //Send Collision Event or Save Collision State
  }

  //[Resolution Phase]
  //resolution_phase->SolveConstraints(manifold_table, &rigid_bodies_, dt);
  {
    //Calculate Forces
    //Solve Velocity Constraints
    //Integrate
    for (auto& body : rigid_bodies_)
    {
      body.Integrate(dt);
      body.UpdateOrientation();
      body.UpdateInertia();
      body.UpdatePosition();
    }
    //Solve Position Constraints
  }
}

void World::LevelEnd()
{
  //rigid_bodies_.clear();
}

SystemOrder World::GetOrder()
{
  return SystemOrder::World;
}

RigidBody* World::GetRigidBody(size_t index)
{
  return &rigid_bodies_[index];
}

RigidBody* World::AddRigidBody()
{
  rigid_bodies_.push_back(RigidBody());
  //initialize general rigid body here when it need.
  return &rigid_bodies_.back();
}
} // namespace Octane
