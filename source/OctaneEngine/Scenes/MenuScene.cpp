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
#include <OctaneEngine/Engine.h>

#include <imgui.h>

#include <iostream>

namespace Octane
{
MenuScene::MenuScene(SceneSys* parent) : IScene(parent), inhand_(*parent->Get<InputHandler>()) {}

void MenuScene::Load() {

}
void MenuScene::Start() {

}
void MenuScene::Update(float dt)
{
  /*if (inhand_.KeyReleased(SDLK_SPACE))
  {
    parent_manager_.SetNextScene(SceneE::TestScene);
  }*/

    ImGui::Begin(
    "menu",
    NULL,
    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::SetWindowPos("menu", ImVec2(800.0f, 500.0f));
  
  if (ImGui::Button("play"))
  {
    parent_manager_.SetNextScene(SceneE::TestScene);
  }
  if (ImGui::Button("quit") || inhand_.KeyReleased(SDLK_ESCAPE))
  {
    parent_manager_.Quit();
  }


  ImGui::End();
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

