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
#include <imgui.h>
#include <OctaneEngine/Style.h>
namespace Octane
{
TestScene::TestScene(SceneSys* parent) : IScene(parent), inhand_(*parent->Get<InputHandler>()) {}

void TestScene::Load() 
{

}
void TestScene::Start() 
{
  esc_menu = false;
  demo_window_open = false;
}
void TestScene::Update(float dt) 
{

 /* if (inhand_.KeyReleased(SDLK_ESCAPE))
  {
    parent_manager_.SetNextScene(SceneE::MenuScene);

  }*/

  ImGui::Begin(
    "Sample Window",
    NULL,
    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

  ImGui::Text(
    "QAWSED for the Red Object Movement\nRFTGYH for the Blue Object\nArrow Keys and Space/Shift for camera movement\nAlt+Enter for Fullscreen");

  ImGui::End();

  //If press the esc, make menu come out
  if (inhand_.KeyReleased(SDLK_ESCAPE))
  {
    esc_menu = true;
  }

  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      if (ImGui::MenuItem("Quit"))
      {
        parent_manager_.Quit();
      }

      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit"))
    {
      if (ImGui::BeginMenu("Theme"))
      {
        if (ImGui::MenuItem("Visual Studio"))
        {
          Style::ImguiVisualStudio();
        }
        if (ImGui::MenuItem("ImGui Dark"))
        {
          ImGui::StyleColorsDark();
        }
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View"))
    {
      if (ImGui::BeginMenu("Windows"))
      {
       // ImGui::MenuItem("Scene Settings", 0, &scene_settings_open);
        ImGui::MenuItem("ImGui Demo", 0, &demo_window_open);
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  /*if (scene_settings_open)
  {
    ImGui::Begin("Scene Settings", &scene_settings_open, ImGuiWindowFlags_AlwaysAutoResize);
    if (ImGui::CollapsingHeader("Light Properties"))
    {
      ImGui::DragFloat3("Light Position", (&light_position.x), 0.01f);
    }
    auto cam_pos = camera.GetPosition();
    ImGui::Text("(%f, %f, %f)", cam_pos.x, cam_pos.y, cam_pos.z);
    ImGui::End();
  }*/

  if (demo_window_open)
  {
    ImGui::ShowDemoWindow(&demo_window_open);
  }

  if (esc_menu)
  {
    ImGui::Begin(
      "menu",
      NULL,
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetWindowPos("menu", ImVec2(800.0f, 500.0f));

    if (ImGui::Button("resume"))
    {
      esc_menu = false;
    }
    if (ImGui::Button("main menu"))
    {
      parent_manager_.SetNextScene(SceneE::MenuScene);
    }
    if (ImGui::Button("quit"))
    {
      parent_manager_.Quit();
    }

    ImGui::End();
  }
}
void TestScene::End() 
{

}
void TestScene::Unload() 
{

}

SceneE TestScene::GetEnum() const  
{
  return SceneE::TestScene;
}
}

