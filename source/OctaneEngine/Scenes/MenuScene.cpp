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
#include <OctaneEngine/WindowManager.h>
#include <OctaneEngine/Engine.h>

#include <imgui.h>
#include <iostream>

#include <OctaneEngine/Graphics/RenderSys.h>

namespace Octane
{
MenuScene::MenuScene(SceneSys* parent) : IScene(parent), inhand_(*parent->Get<InputHandler>()) {}

void MenuScene::Load()
{

}

void MenuScene::Start()
{
  Get<RenderSys>()->SetClearColor(Colors::db32[1]);
}

void MenuScene::Update(float dt)
{
    ImGui::Begin(
    "Main Menu",
    NULL,
    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
  
  ImGui::SetWindowPos(
      "Main Menu",
      ImVec2(0.5f * Get<WindowManager>()->GetWidth(), 0.5f * Get<WindowManager>()->GetHeight()));
  
  if (ImGui::Button("Play"))
  {
    parent_manager_.SetNextScene(SceneE::TestScene);
  }
  if (ImGui::Button("Quit") || inhand_.KeyReleased(SDLK_ESCAPE))
  {
    parent_manager_.Quit();
  }

  ImGui::End();
}

void MenuScene::End()
{

}

void MenuScene::Unload()
{

}

SceneE MenuScene::GetEnum() const
{
  return SceneE::MenuScene;
}
}

