/******************************************************************************/
/*!
  \par        Project Octane
  \file       AbilityTunnelBhv.cpp
  \author     Lowell Novitch
  \date       2020/10/23
  \brief      behavior for wind tunnel spawned by the player

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

// Main include
#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/behaviors/AbilityTunnelBhv.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/BehaviorSys.h>
#include <OctaneEngine/Physics/PhysicsSys.h>
#include <iostream>
namespace Octane
{

AbilityTunnelBhv::AbilityTunnelBhv(BehaviorSys* parent, ComponentHandle handle) : IBehavior(parent, handle) {

}

void AbilityTunnelBhv::Initialize()
{
  //std::cout << "tunnel init" << std::endl;
  
}

void AbilityTunnelBhv::Update(float dt, EntityID myid)
{
  // std::cout << "tunnel update" << std::endl;
  auto enty = Get<EntitySys>();
  auto phys_me = enty->GetEntity(myid).GetComponentHandle(ComponentKind::Physics);

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
          if (Get<PhysicsSys>()->HasCollision(phys_me, phys_other))
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

void AbilityTunnelBhv::Shutdown() {

}

}
