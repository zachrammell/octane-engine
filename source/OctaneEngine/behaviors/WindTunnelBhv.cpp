/******************************************************************************/
/*!
  \par        Project Octane
  \file       WindTunnelBhv.cpp
  \author     Lowell Novitch
  \date       2020/10/23
  \brief      behavior for wind tunnel

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

// Main include
#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/behaviors/WindTunnelBhv.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/BehaviorSys.h>
#include <OctaneEngine/Physics/NarrowPhase.h>
#include <iostream>
namespace Octane
{

WindTunnelBHV::WindTunnelBHV(BehaviorSys* parent, ComponentHandle handle) : IBehavior(parent, handle)
{
  phys_handle_ = INVALID_COMPONENT;
}

void WindTunnelBHV::Initialize() 
{
  //std::cout << "tunnel init" << std::endl;
  auto enty = Get<EntitySys>();

  for (auto it = enty->EntitiesBegin(); it != enty->EntitiesEnd(); ++it)
  {

      //check if compared entity is the current wind tunnel
      if (it->HasComponent(ComponentKind::Behavior))
      {
        auto other = it->GetComponentHandle(ComponentKind::Behavior);

        if (other == handle_)
        {
          //assume this tunnel has a physics component
          phys_handle_ = it->GetComponentHandle(ComponentKind::Physics);
          break;
        }
      }

      
    
  }
}

void WindTunnelBHV::Update(float dt) 
{
 // std::cout << "tunnel update" << std::endl;
  auto enty = Get<EntitySys>();

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
      }
       
      auto phys_me = Get<ComponentSys>()->GetPhysics(phys_handle_);
      auto other_hand = it->GetComponentHandle(ComponentKind::Physics);
      auto phys_other = Get<ComponentSys>()->GetPhysics(other_hand);
      
      NarrowPhase narrow;

      if (narrow.GJKCollisionDetection(phys_me.primitive, phys_other.primitive, Simplex()))
      {
        std::cout << "wind tunnel collide" << std::endl;
      }
    }
  }
}

void WindTunnelBHV::Shutdown()
{
  //std::cout << "tunnel shutdown" << std::endl;
}

}
