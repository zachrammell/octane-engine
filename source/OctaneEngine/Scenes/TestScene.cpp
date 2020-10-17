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


#include <OctaneEngine/EntitySys.h>

#include <OctaneEngine/Engine.h>
#include <OctaneEngine/Graphics/CameraSys.h>
#include <OctaneEngine/Graphics/RenderSys.h>
#include <OctaneEngine/WindowManager.h>
#include <OctaneEngine/InputHandler.h>
#include <OctaneEngine/Scenes/TestScene.h>
#include <OctaneEngine/ImGuiSys.h>
#include <imgui.h>
#include <iostream>

#include <OctaneEngine/Physics/Box.h>
#include <OctaneEngine/Physics/PhysicsSys.h>

namespace dx = DirectX;

namespace
{

Octane::EntityID red_bear_id;
Octane::EntityID blue_bear_id;

struct PhysicsComponentTemp
{
  Octane::RigidBody* rigid_body;
  Octane::Primitive* primitive;
} red_bear_physics, blue_bear_physics;

} // namespace

namespace Octane
{
TestScene::TestScene(SceneSys* parent) : IScene(parent) {}

void TestScene::Load()
{
  auto* entsys = Get<EntitySys>();
  auto* compsys = Get<ComponentSys>();
  for (int i = 0; i < 100; ++i)
  {
    // todo: custom entityid / componentid types with overridden operator*, because this is way too much boilerplate
    EntityID ent_id = entsys->MakeEntity();
    GameEntity& ent = entsys->GetEntity((ent_id));

    ComponentHandle trans_id = compsys->MakeTransform();
    ent.components[to_integral(ComponentKind::Transform)] = trans_id;
    TransformComponent& trans = compsys->GetTransform(trans_id);
    trans.pos.x = 0.25f * (i - 50);
    trans.pos.y = 0.01f * (i - 50) * (i - 50);
    trans.pos.z = 0.33f * (i - 50);
    trans.scale = 0.25f;
    trans.rotation = 0.0f;

    ComponentHandle render_id = compsys->MakeRender();
    ent.components[to_integral(ComponentKind::Render)] = render_id;
    RenderComponent& render_component = compsys->GetRender(render_id);
    render_component.color = Colors::db32[i % 32];
    render_component.mesh_type = (i > 50) ? MeshType::Sphere : MeshType::Cube;
  }

  red_bear_id = Get<EntitySys>()->MakeEntity();

  {
    GameEntity& obj100_entity = Get<EntitySys>()->GetEntity((red_bear_id));
    ComponentHandle trans_id = compsys->MakeTransform();
    obj100_entity.components[to_integral(ComponentKind::Transform)] = trans_id;
    TransformComponent& trans = compsys->GetTransform(trans_id);
    trans.pos.x = 0.0f;
    trans.pos.y = 0.0f;
    trans.pos.z = 0.0f;
    trans.scale = 0.25f;
    trans.rotation = 0.0f;

    ComponentHandle render_comp_id = compsys->MakeRender();
    obj100_entity.components[to_integral(ComponentKind::Render)] = render_comp_id;
    RenderComponent& render_comp = compsys->GetRender(render_comp_id);
    render_comp.color = Colors::red;
    render_comp.mesh_type = MeshType::Bear;
  }

  blue_bear_id = Get<EntitySys>()->MakeEntity();

  {
    GameEntity& obj101_entity = Get<EntitySys>()->GetEntity((blue_bear_id));
    ComponentHandle trans_id = compsys->MakeTransform();
    obj101_entity.components[to_integral(ComponentKind::Transform)] = trans_id;
    TransformComponent& trans = compsys->GetTransform(trans_id);
    trans.pos.x = 0.0f;
    trans.pos.y = 0.0f;
    trans.pos.z = 0.0f;
    trans.scale = 0.25f;
    trans.rotation = 0.0f;

    ComponentHandle render_comp_id = compsys->MakeRender();
    obj101_entity.components[to_integral(ComponentKind::Render)] = render_comp_id;
    RenderComponent& render_comp = compsys->GetRender(render_comp_id);
    render_comp.color = Colors::blue;
    render_comp.mesh_type = MeshType::Bear;
  }

  auto world = Get<PhysicsSys>();

  red_bear_physics.rigid_body = world->AddRigidBody();
  dx::XMFLOAT3 constraints = {1.0f, 1.0f, 1.0f};
  red_bear_physics.rigid_body->SetLinearConstraints(constraints);
  red_bear_physics.rigid_body->SetAngularConstraints(constraints);
  red_bear_physics.primitive = world->AddPrimitive(red_bear_physics.rigid_body, ePrimitiveType::Box);
  static_cast<Box*>(red_bear_physics.primitive)->SetBox(0.25f, 0.25f, 0.25f);

  blue_bear_physics.rigid_body = world->AddRigidBody();
  blue_bear_physics.rigid_body->SetLinearConstraints(constraints);
  blue_bear_physics.rigid_body->SetAngularConstraints(constraints);
  blue_bear_physics.primitive = world->AddPrimitive(blue_bear_physics.rigid_body, ePrimitiveType::Ellipsoid);
}
void TestScene::Start()
{
  esc_menu = false;
  demo_window_open = false;
  Get<RenderSys>()->SetClearColor(Colors::db32[19]);
  SDL_SetRelativeMouseMode(SDL_TRUE);
}
void TestScene::Update(float dt)
{
  ImGui::Begin(
    "Instructions Window",
    NULL,
    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

  ImGui::Text(
    "QAWSED for the Red Object Movement\nRFTGYH for the Blue Object\nWASD and Space/Shift for camera movement\nAlt+Enter for Fullscreen");

  ImGui::End();

  //If press the esc, make menu come out
  if (Get<InputHandler>()->KeyReleased(SDLK_ESCAPE))
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
          ImGuiSys::StyleVisualStudio();
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

  if (demo_window_open)
  {
    ImGui::ShowDemoWindow(&demo_window_open);
  }

  if (esc_menu)
  {
    SDL_SetRelativeMouseMode(SDL_FALSE);

    ImGui::Begin(
      "Pause Menu",
      NULL,
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetWindowPos(
      "Pause Menu",
      ImVec2(0.5f * Get<WindowManager>()->GetWidth(), 0.5f * Get<WindowManager>()->GetHeight()));

    if (ImGui::Button("Resume"))
    {
      esc_menu = false;
      SDL_SetRelativeMouseMode(SDL_TRUE);
    }
    if (ImGui::Button("Main Menu"))
    {
      Get<SceneSys>()->SetNextScene(SceneE::MenuScene);
    }
    if (ImGui::Button("Quit"))
    {
      Get<SceneSys>()->Quit();
    }

    ImGui::End();
  }
  else
  {
    auto* input = Get<InputHandler>();

    //temp physics code

    dx::XMFLOAT3 sample_force;
    sample_force.x = 1.0f * (input->KeyHeld(SDLK_a) - input->KeyHeld(SDLK_d));
    sample_force.y = 1.0f * (input->KeyHeld(SDLK_q) - input->KeyHeld(SDLK_e));
    sample_force.z = 1.0f * (input->KeyHeld(SDLK_w) - input->KeyHeld(SDLK_s));

    dx::XMFLOAT3 sample_force2;
    sample_force2.x = 1.0f * (input->KeyHeld(SDLK_f) - input->KeyHeld(SDLK_h));
    sample_force2.y = 1.0f * (input->KeyHeld(SDLK_r) - input->KeyHeld(SDLK_y));
    sample_force2.z = 1.0f * (input->KeyHeld(SDLK_t) - input->KeyHeld(SDLK_g));

    auto& red_bear_pos
      = Get<ComponentSys>()
          ->GetTransform(Get<EntitySys>()->GetEntity(red_bear_id).GetComponentHandle(ComponentKind::Transform))
          .pos;

    auto& blue_bear_pos
      = Get<ComponentSys>()
          ->GetTransform(Get<EntitySys>()->GetEntity(blue_bear_id).GetComponentHandle(ComponentKind::Transform))
          .pos;

    red_bear_physics.rigid_body->ApplyForceCentroid(sample_force);
    red_bear_physics.rigid_body->SyncToPosition(red_bear_pos);

    blue_bear_physics.rigid_body->ApplyForceCentroid(sample_force2);
    blue_bear_physics.rigid_body->SyncToPosition(blue_bear_pos);

    dx::XMFLOAT3 cam_velocity;
    cam_velocity.x = (input->KeyHeld(SDLK_a) - input->KeyHeld(SDLK_d));
    cam_velocity.y = (input->KeyHeld(SDLK_SPACE) - input->KeyHeld(SDLK_LSHIFT));
    cam_velocity.z = (input->KeyHeld(SDLK_w) - input->KeyHeld(SDLK_s));

    dx::XMStoreFloat3(&cam_velocity, dx::XMVectorScale(dx::XMVector3Normalize(dx::XMLoadFloat3(&cam_velocity)), 0.25f));

    dx::XMINT2 mouse_vel = input->GetMouseMovement();
    auto& camera = Get<CameraSys>()->GetFPSCamera();
    camera.RotatePitchRelative(-mouse_vel.y);
    camera.RotateYawRelative(mouse_vel.x);

    camera.MoveRelativeToView(dx::XMLoadFloat3(&cam_velocity));
  }
}

void TestScene::End()
{
  
}

void TestScene::Unload()
{
  auto* entsys = Get<EntitySys>();
  auto* compsys = Get<ComponentSys>();

  entsys->FreeAllEntities();

  //delete blue_bear_physics.rigid_body;
  //delete blue_bear_physics.primitive;
  //delete red_bear_physics.rigid_body;
  //delete red_bear_physics.primitive;
}

SceneE TestScene::GetEnum() const
{
  return SceneE::TestScene;
}

} // namespace Octane
