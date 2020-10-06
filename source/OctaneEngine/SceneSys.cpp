/******************************************************************************/
/*!
  \par        Project Octane
  \file       SceneSys.cpp
  \author     Lowell Novitch
  \date       2020/10/6
  \brief      Manages the current scene

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

// Main include
#include <OctaneEngine/SceneSys.h>
#include <OctaneEngine/SystemOrder.h>
#include <OctaneEngine/FramerateController.h>

namespace Octane
{
SceneSys::SceneSys(Engine* engine) : ISystem(engine) 
{
    //initialize the starting scene
};

void SceneSys::Load() 
{
  if (current_scene_ != nullptr)
  {
    current_scene_->Load();
  }
}

void SceneSys::LevelStart() 
{
  if (current_scene_ != nullptr)
  {
    current_scene_->Start();
  }
}

void SceneSys::Update() 
{
  if (current_scene_ != nullptr)
  {
    float dt = Get<FramerateController>()->GetDeltaTime();
    current_scene_->Update(dt);
  }
}

void SceneSys::LevelEnd() 
{
  if (current_scene_ != nullptr)
  {
    current_scene_->End();
  }
}

void SceneSys::Unload() 
{
  if (current_scene_ != nullptr)
  {
    current_scene_->Unload();
  }
}

SystemOrder SceneSys::GetOrder()
{
  return SystemOrder::Scene;
}

}
