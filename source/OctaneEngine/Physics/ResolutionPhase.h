#pragma once

#include <OctaneEngine/Math.h>
#include <OctaneEngine/Physics/Constraints.h>
#include <OctaneEngine/Physics/ContactConstraints.h>
#include <OctaneEngine/Components/PhysicsComponent.h>
#include <OctaneEngine/Physics/Force.h>

#include <EASTL/hash_map.h>
#include <EASTL/vector.h>

namespace Octane
{

class ResolutionPhase
{
public:
  ResolutionPhase();
  ~ResolutionPhase() = default;

  void Initialize();
  void Shutdown();
  void Solve(
    eastl::hash_map<size_t, ContactManifold>* manifold_table,
    PhysicsComponent* begin,
    PhysicsComponent* end,
    float dt);

  void AddForce(IForce* force);

private:
  bool is_warm_start_ = false;
  size_t velocity_iteration_ = 8;
  size_t position_iteration_ = 3;
  eastl::vector<ContactConstraints> m_contact_constraints;
  eastl::vector<Constraints*> velocity_constraints_;
  eastl::vector<Constraints*> position_constraints_;
  eastl::vector<IForce*> forces_;
};
} // namespace Octane
