#pragma once

#include <OctaneEngine/Math.h>
#include <OctaneEngine/Physics/ContactConstraints.h>
#include <OctaneEngine/Physics/Constraints.h>

#include <EASTL/vector.h>
#include <EASTL/hash_map.h>

namespace Octane
{
class ResolutionPhase
{
public:
  ResolutionPhase();
  ~ResolutionPhase() = default;

  void Initialize();
  void Shutdown();
  void Solve(eastl::hash_map<size_t, ContactManifold>* manifold_table, eastl::vector<RigidBody*>* rigid_bodies, float dt);

private:
  bool m_b_warm_start = false;
  size_t m_velocity_iteration = 8;
  size_t m_position_iteration = 3;
  eastl::vector<ContactConstraints> m_contact_constraints;
  eastl::vector<Constraints*> m_velocity_constraints;
  eastl::vector<Constraints*> m_position_constraints;
};
} // namespace Octane
