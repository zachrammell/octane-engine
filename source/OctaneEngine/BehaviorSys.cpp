/******************************************************************************/
/*!
  \par        Project Octane
  \file       BehaviorSys.cpp
  \author     Lowell Novitch
  \date       2020/10/6
  \brief      Manages the current scene

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

// Main include
#include <OctaneEngine/BehaviorSys.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/SystemOrder.h>
#include <OctaneEngine/Trace.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/FramerateController.h>

#include <iostream>
#include <magic_enum.hpp>

#include <OctaneEngine/behaviors/WindTunnelBhv.h>
#include <OctaneEngine/behaviors/PlaneBehavior.h>
#include <OctaneEngine/Graphics/CameraSys.h>

namespace Octane
{
BehaviorSys::BehaviorSys(Engine* engine) : ISystem(engine)
{

}

void BehaviorSys::Load()
{
  entsys_ = engine_.GetSystem<EntitySys>();

  for (auto it = entsys_->EntitiesBegin(); it != entsys_->EntitiesEnd(); ++it)
  {
    if (it->HasComponent(ComponentKind::Behavior))
    {
      auto handle = it->GetComponentHandle(ComponentKind::Behavior);
      BehaviorComponent& beh = engine_.GetSystem<ComponentSys>()->GetBehavior(handle);

      if (!beh.initialized)
      {
        if (beh.type == BHVRType::INVALID)
        {
          Trace::Log(WARNING) << "cannot initialize a behavior of INVALID type" << std::endl;
        }
        else
        {
          //initialize stuff
          Trace::Log(DEBUG) << "intialized behavior of type: " << magic_enum::enum_name(beh.type) << std::endl;

          if (beh.behavior == nullptr)
          {

            switch (beh.type)
            {
            case BHVRType::PLAYER:
                beh.behavior = new WindTunnelBHV(this, handle);
                break;
            case BHVRType::WINDTUNNEL: 
                beh.behavior = new WindTunnelBHV(this, handle);
                break;
            case BHVRType::PLANE:
            {
              auto& camera = Get<CameraSys>()->GetFPSCamera();
              beh.behavior = new PlaneBehavior(this, handle,camera.GetViewDirection());
            }
              break;
            default: break;
            }
          }
        }
      }

    }
  }
}

void BehaviorSys::LevelStart()
{
  for (auto it = entsys_->EntitiesBegin(); it != entsys_->EntitiesEnd(); ++it)
  {
    if (it->HasComponent(ComponentKind::Behavior))
    {
      auto handle = it->GetComponentHandle(ComponentKind::Behavior);
      BehaviorComponent& beh = engine_.GetSystem<ComponentSys>()->GetBehavior(handle);

      if (!beh.initialized)
      {
        if (beh.behavior != nullptr)
        {
          beh.behavior->Initialize();
          beh.initialized = true;
        }
      }
    }
  }
}

void BehaviorSys::Update()
{
  float dt = engine_.GetSystem<FramerateController>()->GetDeltaTime();
  for (auto it = entsys_->EntitiesBegin(); it != entsys_->EntitiesEnd(); ++it)
  {
    if (it->HasComponent(ComponentKind::Behavior))
    {
      auto handle = it->GetComponentHandle(ComponentKind::Behavior);
      BehaviorComponent& beh = engine_.GetSystem<ComponentSys>()->GetBehavior(handle);
      //std::cout << magic_enum::enum_name(engine_.GetSystem<ComponentSys>()->GetBehavior(handle).type) << std::endl;

      if (!beh.initialized)
      {
        if (beh.type == BHVRType::INVALID)
        {
          Trace::Log(WARNING) << "cannot initialize a behavior of INVALID type" << std::endl;
        }
        else
        {
          //initialize stuff
          if (beh.behavior == nullptr)
          {

            switch (beh.type)
            {
            case BHVRType::PLAYER: 
                beh.behavior = new WindTunnelBHV(this, handle); 
                break;
            case BHVRType::WINDTUNNEL: 
                beh.behavior = new WindTunnelBHV(this, handle); 
                break;
            case BHVRType::PLANE:
            {
              auto& camera = Get<CameraSys>()->GetFPSCamera();
              beh.behavior = new PlaneBehavior(this, handle, camera.GetViewDirection());
            }
            break;

            default: break;
            }
          }

          beh.behavior->Initialize();
        }

        beh.initialized = true;
      }

      if (beh.type != BHVRType::INVALID)
      {
        beh.behavior->Update(dt);
        
      }
    }
  }
}

void BehaviorSys::LevelEnd()
{
  if (entsys_ == nullptr)
  {
    return;
  }
  for (auto it = entsys_->EntitiesBegin(); it != entsys_->EntitiesEnd(); ++it)
  {
    if (it->HasComponent(ComponentKind::Behavior))
    {
      auto handle = it->GetComponentHandle(ComponentKind::Behavior);
      BehaviorComponent& beh = engine_.GetSystem<ComponentSys>()->GetBehavior(handle);

      if (beh.initialized)
      {
        if (beh.behavior != nullptr)
        {
          beh.behavior->Shutdown();
          beh.initialized = false;
        }
      }
    }
  }
}

void BehaviorSys::Unload()
{
  if (entsys_ == nullptr)
  {
    return;
  }
    //unload initialized behaviors
  for (auto it = entsys_->EntitiesBegin(); it != entsys_->EntitiesEnd(); ++it)
  {
    if (it->HasComponent(ComponentKind::Behavior))
    {
      auto handle = it->GetComponentHandle(ComponentKind::Behavior);
      BehaviorComponent& beh = engine_.GetSystem<ComponentSys>()->GetBehavior(handle);

      if (beh.behavior != nullptr)
      {
        delete beh.behavior;
      }
    }
  }
}

SystemOrder BehaviorSys::GetOrder()
{
  return SystemOrder::Behavior;
}




}
