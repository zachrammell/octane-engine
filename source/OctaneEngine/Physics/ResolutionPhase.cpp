#include <OctaneEngine/Physics/ResolutionPhase.h>


namespace Octane
{
  ResolutionPhase::ResolutionPhase()
{
}

void ResolutionPhase::Initialize()
{
}

void ResolutionPhase::Shutdown()
{
  for (auto& constraint : m_velocity_constraints)
  {
    constraint->Release();
    delete constraint;
    constraint = nullptr;
  }
  m_velocity_constraints.clear();
  for (auto& constraint : m_position_constraints)
  {
    constraint->Release();
    delete constraint;
    constraint = nullptr;
  }
  m_position_constraints.clear();
}

void ResolutionPhase::Solve(
  eastl::hash_map<size_t, ContactManifold>* manifold_table,
  eastl::vector<RigidBody*>* rigid_bodies,
  float dt)
{
}
}
