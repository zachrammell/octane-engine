/******************************************************************************/
/*!
  \par        Project Octane
  \file       BehaviorSys.cpp
  \author     Lowell Novitch
  \date       2020/10/6
  \brief      Manages the current scene

  Copyright � 2020 DigiPen, All rights reserved.
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

#include <OctaneEngine/behaviors/IBehavior.h>


namespace Octane
{
BehaviorSys::BehaviorSys(Engine* engine) : ISystem(engine)
{

}

void BehaviorSys::Load()
{
  entsys_ = engine_.GetSystem<EntitySys>();

  
}

void BehaviorSys::LevelStart()
{
  for (auto it = entsys_->EntitiesBegin(); it != entsys_->EntitiesEnd(); ++it)
  {
    if (it->active && it->HasComponent(ComponentKind::Behavior))
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

  if (dt<= 0)
  {
    return;
  }

  for (auto it = entsys_->EntitiesBegin(); it != entsys_->EntitiesEnd(); ++it)
  {
    if (it->active && it->HasComponent(ComponentKind::Behavior))
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

        beh.behavior->Initialize();
        beh.initialized = true;
      }

      if (beh.type != BHVRType::INVALID && beh.behavior != nullptr)
      {
        EntityID id = it.ID();
        beh.behavior->Update(dt, id);
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
    if (it->active && it->HasComponent(ComponentKind::Behavior))
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
 
}

SystemOrder BehaviorSys::GetOrder()
{
  return SystemOrder::BehaviorSys;
}

}
