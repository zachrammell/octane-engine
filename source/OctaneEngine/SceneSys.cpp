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
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/SystemOrder.h>
#include <OctaneEngine/FramerateController.h>
#include <OctaneEngine/Trace.h>
#include <iostream>

namespace Octane
{
SceneSys::SceneSys(Engine* engine) : ISystem(engine), frc_(*engine->GetSystem<FramerateController>())
{
    //such a bad way of doing this but it is fine for now
  scene_holder_.push_back(new MenuScene(this));
  scene_holder_.push_back(new TestScene(this));
    //initialize the starting scene
  next_scene_ = scene_holder_[0];

}

void SceneSys::Load() 
{
  current_scene_ = next_scene_;
  next_scene_ = nullptr;
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
    
    float dt = frc_.GetDeltaTime();
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

void SceneSys::SetNextScene(SceneE next_scene) 
{
  if (next_scene < SceneE::COUNT)
  {
    next_scene_ = scene_holder_[static_cast<unsigned int>(next_scene)];
  }

  Trace::Log(DEBUG) << "switched to scene #" << static_cast<unsigned int>(next_scene) << std::endl;

  engine_.ChangeScene();
}

SceneE SceneSys::GetCurrentScene() const 
{
  return current_scene_->GetEnum();
}

void SceneSys::Quit() 
{
  engine_.Quit();
}
}
