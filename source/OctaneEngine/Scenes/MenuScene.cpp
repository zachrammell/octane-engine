/******************************************************************************/
/*!
  \par        Project Octane
  \file       MenuScene.cpp
  \author     Lowell Novitch
  \date       2020/10/16
  \brief      menu scene

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

// Main include
#include <OctaneEngine/Scenes/MenuScene.h>
#include <OctaneEngine/InputHandler.h>
#include <iostream>
#include <OctaneEngine/Engine.h>
namespace Octane
{
MenuScene::MenuScene(SceneSys* parent) : IScene(parent), inhand_(*parent->Get<InputHandler>()) {}

void MenuScene::Load() {

}
void MenuScene::Start() {

}
void MenuScene::Update(float dt)
{
  if (inhand_.KeyReleased(SDLK_SPACE))
  {
    parent_manager_.SetNextScene(SceneE::TestScene);
  }
}
void MenuScene::End() {

}
void MenuScene::Unload() {

}

std::string MenuScene::GetName() const
{
  return "MenuScene";
}
}

