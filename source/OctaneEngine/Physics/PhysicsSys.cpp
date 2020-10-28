#include <OctaneEngine/Physics/PhysicsSys.h>

#include <iostream>

#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/FramerateController.h>
#include <OctaneEngine/Physics/Box.h>
#include <OctaneEngine/Physics/Capsule.h>
#include <OctaneEngine/Physics/Ellipsoid.h>
#include <OctaneEngine/Physics/Simplex.h>
#include <OctaneEngine/Physics/Truncated.h>
#include <OctaneEngine/SystemOrder.h>

#include <OctaneEngine/Trace.h>

namespace Octane
{
PhysicsSys::PhysicsSys(Engine* engine) : ISystem(engine) {}

void PhysicsSys::LevelStart() {}

void PhysicsSys::Update()
{
  float dt = 1.0f / 60.0f;

  auto* component_sys = Get<ComponentSys>();
  for (GameEntity* entity = Get<EntitySys>()->EntitiesBegin(); entity != Get<EntitySys>()->EntitiesEnd(); ++entity)
  {
    if (entity->active && entity->HasComponent(ComponentKind::Transform) && entity->HasComponent(ComponentKind::Physics))
    {
      auto& transform = component_sys->GetTransform(entity->GetComponentHandle(ComponentKind::Transform));
      auto& physics_component = component_sys->GetPhysics(entity->GetComponentHandle(ComponentKind::Physics));

      physics_component.rigid_body.SetPosition(transform.pos);
      physics_component.rigid_body.SetOrientation(transform.rotation);
    }
  }

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
  auto* physics_begin = component_sys->PhysicsBegin();
  auto* physics_end = component_sys->PhysicsEnd();
  resolution_phase_.Solve(&manifold_table_, physics_begin, physics_end, dt);

  //copy physics calculation to transform
   for (GameEntity* entity = Get<EntitySys>()->EntitiesBegin(); entity != Get<EntitySys>()->EntitiesEnd(); ++entity)
  {
    if (entity->active && entity->HasComponent(ComponentKind::Transform) && entity->HasComponent(ComponentKind::Physics))
    {
      auto& transform = component_sys->GetTransform(entity->GetComponentHandle(ComponentKind::Transform));
      auto& physics_component = component_sys->GetPhysics(entity->GetComponentHandle(ComponentKind::Physics));

      transform.pos = physics_component.rigid_body.GetPosition();
      transform.rotation = physics_component.rigid_body.GetOrientation();
    }
  }
}

void PhysicsSys::LevelEnd()
{
  //TODO clear world!
}

SystemOrder PhysicsSys::GetOrder()
{
  return SystemOrder::World;
}

void PhysicsSys::InitializeRigidBody(PhysicsComponent& compo)
{
  //linear data - positional
  compo.rigid_body.position_ = DirectX::XMVECTOR();
  compo.rigid_body.linear_velocity_ = DirectX::XMVECTOR();
  compo.rigid_body.force_accumulator_ = DirectX::XMVECTOR();
  compo.rigid_body.linear_constraints_ = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);

  //angular data - rotational
  compo.rigid_body.orientation_ = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
  compo.rigid_body.inverse_orientation_ = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
  compo.rigid_body.angular_velocity_ = DirectX::XMVECTOR();
  compo.rigid_body.torque_accumulator_ = DirectX::XMVECTOR();
  compo.rigid_body.angular_constraints_ = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);

  //mass data
  compo.rigid_body.mass_data_ = MassData();
  compo.rigid_body.global_centroid_ = DirectX::XMVECTOR();
  compo.rigid_body.global_inertia_ = DirectX::XMMatrixIdentity();
  compo.rigid_body.global_inverse_inertia_ = DirectX::XMMatrixIdentity();
}

void PhysicsSys::AddPrimitive(PhysicsComponent& compo, ePrimitiveType type)
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
    primitive->rigid_body_ = &compo.rigid_body;
    primitives_.push_back(primitive);
    compo.primitive = primitive;
  }
}
} // namespace Octane
