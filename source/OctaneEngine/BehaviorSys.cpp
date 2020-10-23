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

#include <iostream>
#include <magic_enum.hpp>

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

}

void BehaviorSys::Update()
{
  for (auto it = entsys_->EntitiesBegin(); it != entsys_->EntitiesEnd(); ++it)
  {
    if (it->HasComponent(ComponentKind::Behavior))
    {
     // auto handle = it->GetComponentHandle(ComponentKind::Behavior);
      //std::cout << magic_enum::enum_name(engine_.GetSystem<ComponentSys>()->GetBehavior(handle).type) << std::endl;
    }
  }
}

void BehaviorSys::LevelEnd()
{

}

void BehaviorSys::Unload()
{

}

SystemOrder BehaviorSys::GetOrder()
{
  return SystemOrder::Behavior;
}




}
