#include <OctaneEngine/Physics/ResolutionPhase.h>

namespace Octane
{
ResolutionPhase::ResolutionPhase() {}

void ResolutionPhase::Initialize() {}

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
  PhysicsComponent* begin,
  PhysicsComponent* end,
  float dt)
{
  //Apply Forces
  //for (auto& force : forces)
  //{
  //  for (auto& body : *rigid_bodies)
  //  {
  //    force->Update(body, dt);
  //  }
  //}
  m_contact_constraints.clear();
  if (m_velocity_iteration > 0)
  {
    for (auto& constraint : m_velocity_constraints)
    {
      constraint->Generate(dt);
    }
    for (auto& manifold : *manifold_table)
    {
      auto& contact = m_contact_constraints.emplace_back(&manifold.second, 0.1f, 0.1f);
      contact.Generate(dt);
    }
    if (m_b_warm_start == true)
    {
      for (auto& contact_constraint : m_contact_constraints)
      {
        contact_constraint.WarmStart();
      }
    }
    for (size_t i = 0; i < m_velocity_iteration; ++i)
    {
      for (auto& constraint : m_velocity_constraints)
      {
        constraint->Solve(dt);
      }
      for (auto& contact : m_contact_constraints)
      {
        contact.Solve(dt);
      }
    }
    for (auto& constraint : m_velocity_constraints)
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

  if (m_position_iteration > 0)
  {
    for (auto& constraint : m_position_constraints)
    {
      constraint->Generate(dt);
    }
    for (size_t i = 0; i < m_position_iteration; ++i)
    {
      for (auto& constraint : m_position_constraints)
      {
        constraint->Solve(dt);
      }
    }
    for (auto& constraint : m_position_constraints)
    {
      constraint->Apply();
    }
  }
}
} // namespace Octane
