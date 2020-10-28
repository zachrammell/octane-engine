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

#include <OctaneEngine/Engine.h>
#include <OctaneEngine/InputHandler.h>
#include <OctaneEngine/WindowManager.h>

#include <EASTL/string_view.h>
#include <imgui.h>
#define MAGIC_ENUM_USING_ALIAS_STRING_VIEW using string_view = eastl::string_view;
#include <EASTL/array.h>
#include <magic_enum.hpp>

#include <OctaneEngine/Graphics/RenderSys.h>


namespace Octane
{
MenuScene::MenuScene(SceneSys* parent) : IScene(parent), selected_scene_ {SceneE::TestScene} {}

void MenuScene::Load() {}

void MenuScene::Start()
{
  Get<RenderSys>()->SetClearColor(Colors::db32[1]);
}

void MenuScene::Update(float dt)
{
  ImGuiWindowFlags const flags
    = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize;
  int const w = Get<WindowManager>()->GetWidth();
  int const h = Get<WindowManager>()->GetHeight();
  if (ImGui::Begin("Main Menu", NULL, flags))
  {

    ImGui::SetWindowPos(ImVec2(0.5f * w, 0.5f * h), ImGuiCond_Always);

    if (ImGui::Button("Play"))
    {
      parent_manager_.SetNextScene(selected_scene_);
    }
    if (ImGui::Button("Quit") || Get<InputHandler>()->KeyReleased(SDLK_ESCAPE))
    {
      parent_manager_.Quit();
    }
  }
  ImGui::End();

  if (ImGui::Begin("Scene Picker", nullptr, flags))
  {
    ImGui::SetWindowPos({0.8f *w, 0.5f * h}, ImGuiCond_Always);
    if (ImGui::BeginCombo("Scene", magic_enum::enum_name(selected_scene_).data(), ImGuiComboFlags_None))
    {
      for (auto const& scene : magic_enum::enum_entries<SceneE>())
      {
        if (scene.first == SceneE::COUNT || scene.first == GetEnum())
        {
          continue;
        }
        if (ImGui::Selectable(scene.second.data()))
        {
          selected_scene_ = scene.first;
        }
      }
      ImGui::EndCombo();
    }
  }
  ImGui::End();
}

void MenuScene::End() {}

void MenuScene::Unload() {}

SceneE MenuScene::GetEnum() const
{
  return SceneE::MenuScene;
}
} // namespace Octane
