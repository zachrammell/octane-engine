/******************************************************************************/
/*!
  \par        Project Octane
  \file       AbilityHomingBhv.cpp
  \author     Lowell Novitch
  \date       2020/10/23
  \brief      behavior for homing wind tunnel spawned by the player

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

// Main include
#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/behaviors/AbilityHomingBhv.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/BehaviorSys.h>
#include <OctaneEngine/Physics/PhysicsSys.h>
#include <iostream>
namespace Octane
{

AbilityHomingBhv::AbilityHomingBhv(BehaviorSys* parent, ComponentHandle handle) : IBehavior(parent, handle) {

}

void AbilityHomingBhv::Initialize()
{
  //std::cout << "tunnel init" << std::endl;
  
}

void AbilityHomingBhv::Update(float dt, EntityID myid)
{
  // std::cout << "tunnel update" << std::endl;
  auto enty = Get<EntitySys>();
  auto phys_me = enty->GetEntity(myid).GetComponentHandle(ComponentKind::Physics);

  Get<ComponentSys>()->GetBehavior(handle_).force = GetForce(myid);

  for (auto it = enty->EntitiesBegin(); it != enty->EntitiesEnd(); ++it)
  {
    if (it->HasComponent(ComponentKind::Physics))
    {
      //check if compared entity is the current wind tunnel
      if (it->HasComponent(ComponentKind::Behavior))
      {
        auto other = it->GetComponentHandle(ComponentKind::Behavior);

        if (other == handle_)
        {
          continue;
        }

        const auto& othbeh = Get<ComponentSys>()->GetBehavior(other);

        if (othbeh.type == BHVRType::PLANE)
        {
          auto phys_other = it->GetComponentHandle(ComponentKind::Physics);

          //Apply force to paper airplane
          if (Get<PhysicsSys>()->HasCollision( phys_me,phys_other))
          {
            Get<ComponentSys>()->GetPhysics(phys_other).ApplyForce(Get<ComponentSys>()->GetBehavior(handle_).force);
          }
        }
      }
    }
  }

  life_time_ -= dt;

  if (life_time_ <= 0)
  {
    enty->FreeEntity(myid);
  }
}

void AbilityHomingBhv::Shutdown() {

}

DirectX::XMFLOAT3 AbilityHomingBhv::GetForce(EntityID myid) 
{
  auto entsys = Get<EntitySys>();

  GameEntity& wind_tunnel_entity = entsys->GetEntity(myid);
  ComponentHandle wind_behavior = wind_tunnel_entity.GetComponentHandle(ComponentKind::Behavior);
  BehaviorComponent& beh_comp = Get<ComponentSys>()->GetBehavior(wind_behavior);
  ComponentHandle wind_transform = wind_tunnel_entity.GetComponentHandle(ComponentKind::Transform);
  TransformComponent& wind_transform_comp = Get<ComponentSys>()->GetTransform(wind_transform);

  float closest_dist = FLT_MAX;

  DirectX::XMVECTOR wind_pos = DirectX::XMLoadFloat3(&wind_transform_comp.pos);
  DirectX::XMVECTOR closest_plane = wind_pos;
  for (auto it = entsys->EntitiesBegin(); it != entsys->EntitiesEnd(); ++it)
  {
    if (it->HasComponent(ComponentKind::Behavior))
    {
      auto other = it->GetComponentHandle(ComponentKind::Behavior);
      const auto& othbeh = Get<ComponentSys>()->GetBehavior(other);

      if (othbeh.type == BHVRType::PLANE)
      {
        ComponentHandle plane_transform = it->GetComponentHandle(ComponentKind::Transform);
        TransformComponent& plane_transform_comp = Get<ComponentSys>()->GetTransform(plane_transform);
        DirectX::XMVECTOR plane_pos = DirectX::XMLoadFloat3(&plane_transform_comp.pos);
        DirectX::XMVECTOR wind_to_plane = DirectX::XMVectorSubtract(plane_pos, wind_pos);
        float const distance = DirectX::XMVector3LengthSq(wind_to_plane).m128_f32[0];
        if (distance < closest_dist)
        {
          closest_plane = plane_pos;
          closest_dist = distance;
        }
      }
    }
  }

  DirectX::XMFLOAT3 wind_dir = {-100.f, 30.f, 0.f}; //serialize this later
  closest_dist = FLT_MAX;

  for (auto it = entsys->EntitiesBegin(); it != entsys->EntitiesEnd(); ++it)
  {
    if (it->HasComponent(ComponentKind::Behavior))
    {
      auto other = it->GetComponentHandle(ComponentKind::Behavior);
      const auto& othbeh = Get<ComponentSys>()->GetBehavior(other);

      if (othbeh.type == BHVRType::BEAR || othbeh.type == BHVRType::DUCK || othbeh.type == BHVRType::BUNNY)
      {
        ComponentHandle enemy_transform = it->GetComponentHandle(ComponentKind::Transform);
        TransformComponent& enemy_transform_comp = Get<ComponentSys>()->GetTransform(enemy_transform);
        DirectX::XMVECTOR plane_to_enemy
          = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&enemy_transform_comp.pos), closest_plane);
        float const distance = DirectX::XMVector3LengthSq(plane_to_enemy).m128_f32[0];
        if (distance < closest_dist)
        {
          float wind_strength = 20.0f; //serialize this later
          DirectX::XMVECTOR plane_flight
            = DirectX::XMVectorScale(DirectX::XMVector3Normalize(plane_to_enemy), wind_strength);
          DirectX::XMStoreFloat3(&wind_dir, DirectX::XMVectorAdd(plane_flight, {0, 9.81f * 2.0f / (wind_strength), 0}));
          closest_dist = distance;
        }
      }
    }
  }

  return wind_dir;
  
}

}
