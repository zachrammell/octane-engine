/******************************************************************************/
/*!
  \par        Project Octane
  \file       TestScene.cpp
  \author     Lowell Novitch
  \date       2020/10/6
  \brief      test scene

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

// Main include
#include <OctaneEngine/Scenes/TestScene.h>
#include <OctaneEngine/InputHandler.h>
#include <iostream>
#include <OctaneEngine/Engine.h>
namespace Octane
{
TestScene::TestScene(SceneSys* parent) : IScene(parent), inhand_(*parent->Get<InputHandler>()) {}

void TestScene::Load() 
{

}
void TestScene::Start() 
{

}
void TestScene::Update(float dt) 
{

  if (inhand_.KeyReleased(SDLK_SPACE))
  {
    parent_manager_.SetNextScene(SceneE::MenuScene);
  }

}
void TestScene::End() 
{

}
void TestScene::Unload() 
{

}

std::string TestScene::GetName() const 
{
  return "TestScene";
}
}

