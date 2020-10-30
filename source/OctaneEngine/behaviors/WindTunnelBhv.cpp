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
#include <OctaneEngine/Physics/PhysicsSys.h>
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
          trans_handle_ = it->GetComponentHandle(ComponentKind::Transform);
          break;
        }
      }

      
    
  }
}
int gol = 0;
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

        auto othbeh = Get<ComponentSys>()->GetBehavior(other);

        if (othbeh.type == BHVRType::PLANE)
        {
          auto& phys_me = Get<ComponentSys>()->GetPhysics(phys_handle_);
          auto& trans_me = Get<ComponentSys>()->GetTransform(trans_handle_);
          auto other_hand = it->GetComponentHandle(ComponentKind::Physics);
          auto& phys_other = Get<ComponentSys>()->GetPhysics(other_hand);
          auto& trans_other = Get<ComponentSys>()->GetTransform(it->GetComponentHandle(ComponentKind::Transform));


          if (Get<PhysicsSys>()->HasCollision(trans_me,phys_me.primitive,trans_other,phys_other.primitive))
          {
            //std::cout << "wind tunnel collide " << gol++ << std::endl;
            phys_other.rigid_body.ApplyForceCentroid(Get<ComponentSys>()->GetBehavior(handle_).force);
          }
        }
      }
       
      
    }
  }
}

void WindTunnelBHV::Shutdown()
{
  //std::cout << "tunnel shutdown" << std::endl;
}

}
