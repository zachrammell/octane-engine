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
namespace Octane
{
MenuScene::MenuScene(SceneSys* parent) : IScene(parent) {}

void MenuScene::Load() {

}
void MenuScene::Start() {

}
void MenuScene::Update(float dt)
{
 // auto* input = Get<InputHandler>();

  //if (input->KeyReleased(SDLK_SPACE))
  //{
  //  std::cout << "hello" << std::endl;
 // }

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

