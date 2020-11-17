#include <OctaneEngine/Physics/PhysicsSys.h>

#include <iostream>

#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/FramerateController.h>
#include <OctaneEngine/Physics/Box.h>
#include <OctaneEngine/Physics/Capsule.h>
#include <OctaneEngine/Physics/DragForce.h>
#include <OctaneEngine/Physics/Ellipsoid.h>
#include <OctaneEngine/Physics/GlobalGravityForce.h>
#include <OctaneEngine/Physics/Simplex.h>
#include <OctaneEngine/Physics/Truncated.h>
#include <OctaneEngine/SystemOrder.h>

#include <OctaneEngine/Trace.h>

namespace Octane
{
PhysicsSys::PhysicsSys(Engine* engine) : ISystem(engine) {}

void PhysicsSys::LevelStart()
{
  //resolution_phase_.AddForce(new GlobalGravityForce({0.0f, -9.81f, 0.0f}));
  resolution_phase_.AddForce(new DragForce(0.15f, 0.15f));
}

void PhysicsSys::Update()
{
  float dt = Get<FramerateController>()->GetDeltaTime(); //1.0f / 60.0f;

  auto* component_sys = Get<ComponentSys>();
  for (auto entity = Get<EntitySys>()->EntitiesBegin(); entity != Get<EntitySys>()->EntitiesEnd(); ++entity)
  {
    if (
      entity->active && entity->HasComponent(ComponentKind::Transform) && entity->HasComponent(ComponentKind::Physics))
    {
      auto& transform = component_sys->GetTransform(entity->GetComponentHandle(ComponentKind::Transform));
      auto& physics_component = component_sys->GetPhysics(entity->GetComponentHandle(ComponentKind::Physics));

      physics_component.primitive->rigid_body_ = &physics_component.rigid_body;
      physics_component.sys = this;
      //physics_component.rigid_body.SetPosition(transform.pos);
      //physics_component.rigid_body.SetOrientation(transform.rotation);
    }
  }

  //Three stage of physics pipeline

  //[Broad Phase]
  //broad_phase->Update(dt);
  //broad_phase->ComputePairs(pairs);

  //N^2 Brute Force Broad Phase
  potential_pairs_.clear();
  auto primitive_end = primitives_.end();
  auto entsys = Get<EntitySys>();
  for (auto it = primitives_.begin(); it != primitive_end; ++it)
  {
    auto jt_begin = it;
    for (auto jt = ++jt_begin; jt != primitive_end; ++jt)
    {
      Primitive* collider_a = (*it);
      Primitive* collider_b = (*jt);
      if (collider_a != nullptr && collider_b != nullptr)
        potential_pairs_.emplace_back(collider_a, collider_b);
    }
  }

  //const auto end = entsys->EntitiesEnd();

  //for (auto it = entsys->EntitiesBegin(); it != end; ++it)
  //{
  //  if (it->HasComponent(ComponentKind::Physics))
  //  {
  //    auto jt_begin = it;
  //    for (auto jt = ++jt_begin; jt != end; ++jt)
  //    {
  //      if (jt->HasComponent(ComponentKind::Physics))
  //      {

  //        Primitive* collider_a = component_sys->GetPhysics(it->components[to_integral(ComponentKind::Physics)]).primitive;
  //        Primitive* collider_b = component_sys->GetPhysics(jt->components[to_integral(ComponentKind::Physics)]).primitive;

  //        potential_pairs_.emplace_back(collider_a, collider_b);
  //      }
  //    }
  //  }
  //}

  //[Narrow Phase]
  narrow_phase_.GenerateContact(potential_pairs_, &manifold_table_, &collision_table_);

  //[Resolution Phase]
  auto* physics_begin = component_sys->PhysicsBegin();
  auto* physics_end = component_sys->PhysicsEnd();
  resolution_phase_.Solve(&manifold_table_, physics_begin, physics_end, dt);

  //copy physics calculation to transform
  for (auto entity = Get<EntitySys>()->EntitiesBegin(); entity != Get<EntitySys>()->EntitiesEnd(); ++entity)
  {
    if (
      entity->active && entity->HasComponent(ComponentKind::Transform) && entity->HasComponent(ComponentKind::Physics))
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
  potential_pairs_.clear();
  manifold_table_.clear();
  collision_table_.clear();
  for (auto& primitive : primitives_)
  {
    delete primitive;
    primitive = nullptr;
  }
  primitives_.clear();
}

SystemOrder PhysicsSys::GetOrder()
{
  return SystemOrder::PhysicsSys;
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
  compo.rigid_body.is_dynamic_ = true;
  compo.rigid_body.is_ghost_ = false;
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
    compo.sys = this;
  }
}

void PhysicsSys::ErasePrimitive(PhysicsComponent& compo)
{
  // this helper function actually calls remove & erase, so the entire vector won't have to be shifted
  // (it just swaps the target element to the end)
  // but order is not preserved
  eastl::erase(primitives_, compo.primitive);

  if (compo.primitive != nullptr)
  {
    delete compo.primitive;
    compo.primitive = nullptr;
  }
}

eCollisionState PhysicsSys::HasCollision(PhysicsComponent& a, PhysicsComponent& b) const
{
  size_t key = reinterpret_cast<size_t>(a.primitive) + reinterpret_cast<size_t>(b.primitive);
  auto collision = collision_table_.find(key);
  if (collision != collision_table_.end())
  {
    return collision->second;
  }
  return eCollisionState::None;
}

bool PhysicsSys::HasCollision(
  const TransformComponent& transform_a,
  Primitive* primitive_a,
  const TransformComponent& transform_b,
  Primitive* primitive_b,
  size_t exit_count)
{
  Simplex simplex;
  return narrow_phase_.GJKCollisionDetection(transform_a, primitive_a, transform_b, primitive_b, exit_count, simplex);
}
} // namespace Octane
