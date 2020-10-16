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
#include <OctaneEngine/Scenes/TestScene.h>
#include <OctaneEngine/Scenes/MenuScene.h>

namespace Octane
{
SceneSys::SceneSys(Engine* engine) : ISystem(engine) 
{
    //such a bad way of doing this but it is fine for now
  scene_holder_.push_back(new TestScene(this));
  scene_holder_.push_back(new MenuScene(this));
    //initialize the starting scene
  current_scene_ = scene_holder_[0];
}

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
    float dt = 1.0f / 60.0f; ///Get<FramerateController>()->GetDeltaTime();
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
