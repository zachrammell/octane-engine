#include <OctaneEngine/Physics/ResolutionPhase.h>

namespace Octane
{
ResolutionPhase::ResolutionPhase() {}

void ResolutionPhase::Initialize() {}

void ResolutionPhase::Shutdown()
{
  for (auto& constraint : velocity_constraints_)
  {
    constraint->Release();
    delete constraint;
    constraint = nullptr;
  }
  velocity_constraints_.clear();
  for (auto& constraint : position_constraints_)
  {
    constraint->Release();
    delete constraint;
    constraint = nullptr;
  }
  position_constraints_.clear();

  for (auto& force : forces_)
  {
    delete force;
    force = nullptr;
  }
  forces_.clear();
}

void ResolutionPhase::Solve(
  eastl::hash_map<size_t, ContactManifold>* manifold_table,
  PhysicsComponent* begin,
  PhysicsComponent* end,
  float dt)
{
  //Apply Forces
  for (auto& force : forces_)
  {
    for (auto it = begin; it != end; ++it)
    {
      force->Solve(it->rigid_body, dt);
    }
  }
  m_contact_constraints.clear();
  if (velocity_iteration_ > 0)
  {
    for (auto& constraint : velocity_constraints_)
    {
      constraint->Generate(dt);
    }
    for (auto& manifold : *manifold_table)
    {
      auto& contact = m_contact_constraints.emplace_back(&manifold.second, 0.1f, 0.1f);
      contact.Generate(dt);
    }
    if (is_warm_start_ == true)
    {
      for (auto& contact_constraint : m_contact_constraints)
      {
        contact_constraint.WarmStart();
      }
    }
    for (size_t i = 0; i < velocity_iteration_; ++i)
    {
      for (auto& constraint : velocity_constraints_)
      {
        constraint->Solve(dt);
      }
      for (auto& contact : m_contact_constraints)
      {
        contact.Solve(dt);
      }
    }
    for (auto& constraint : velocity_constraints_)
    {
      constraint->Apply();
    }
    for (auto& contact : m_contact_constraints)
    {
      contact.Apply();
    }
  }

  for (auto it = begin; it != end; ++it)
  {
    it->rigid_body.Integrate(dt);
    it->rigid_body.UpdateOrientation();
    it->rigid_body.UpdateInertia();
    it->rigid_body.UpdatePosition();
  }

  if (position_iteration_ > 0)
  {
    for (auto& constraint : position_constraints_)
    {
      constraint->Generate(dt);
    }
    for (size_t i = 0; i < position_iteration_; ++i)
    {
      for (auto& constraint : position_constraints_)
      {
        constraint->Solve(dt);
      }
    }
    for (auto& constraint : position_constraints_)
    {
      constraint->Apply();
    }
  }
}

void ResolutionPhase::AddForce(IForce* force)
{
  forces_.push_back(force);
}
} // namespace Octane
