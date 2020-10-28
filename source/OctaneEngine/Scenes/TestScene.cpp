/******************************************************************************/
/*!
  \par        Project Octane
  \file       TestScene.cpp
  \author     Lowell Novitch
  \date       2020/10/6
  \brief      test scene

  Copyright � 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

// Main include

#include <OctaneEngine/EntitySys.h>

#include <OctaneEngine/Engine.h>
#include <OctaneEngine/Graphics/CameraSys.h>
#include <OctaneEngine/Graphics/RenderSys.h>
#include <OctaneEngine/ImGuiSys.h>
#include <OctaneEngine/InputHandler.h>
#include <OctaneEngine/Scenes/TestScene.h>
#include <OctaneEngine/WindowManager.h>
#include <imgui.h>
#include <iostream>

#include <OctaneEngine/Physics/Box.h>
#include <OctaneEngine/Physics/PhysicsSys.h>

#include <OctaneEngine/TransformHelpers.h>

namespace dx = DirectX;

namespace
{

Octane::EntityID bear_id;
Octane::EntityID duck_id;
Octane::EntityID bunny_id;
Octane::EntityID wind_tunnel_id;
const dx::XMFLOAT3 PHYSICS_CONSTRAINTS = {1.0f, 1.0f, 1.0f};

//struct PhysicsComponentTemp
//{
//  Octane::RigidBody* rigid_body;
//  Octane::Primitive* primitive;
//} bear_physics, duck_physics, bunny_physics, wind_tunnel_physics;
//
} // namespace

namespace Octane
{
TestScene::TestScene(SceneSys* parent) : IScene(parent) {}

void TestScene::Load()
{
  auto* entsys = Get<EntitySys>();
  auto* compsys = Get<ComponentSys>();
  auto* physics_sys = Get<PhysicsSys>();

  auto create_object = [=](dx::XMFLOAT3 pos, dx::XMFLOAT3 scale, Color color, MeshType mesh_type = MeshType::Cube) {
    // todo: custom entityid / componentid types with overridden operator*, because this is way too much boilerplate
    EntityID const ent_id = entsys->MakeEntity();
    GameEntity& ent = entsys->GetEntity((ent_id));
    ComponentHandle const trans_id = compsys->MakeTransform();
    ent.components[to_integral(ComponentKind::Transform)] = trans_id;
    TransformComponent& trans = compsys->GetTransform(trans_id);
    trans.pos = pos;
    trans.scale = scale;
    trans.rotation = {};
    ComponentHandle const render_id = compsys->MakeRender();
    ent.components[to_integral(ComponentKind::Render)] = render_id;
    RenderComponent& render_component = compsys->GetRender(render_id);
    render_component.color = color;
    render_component.mesh_type = mesh_type;
  };

  // ground plane
  create_object({0.0f, 0.0f, 0.0f}, {30.0f, 0.25f, 30.0f}, Colors::db32[10]);
  create_object({0.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, Colors::db32[25]);

  bear_id = Get<EntitySys>()->MakeEntity();

  {
    GameEntity& bear = Get<EntitySys>()->GetEntity((bear_id));
    ComponentHandle trans_id = compsys->MakeTransform();
    bear.components[to_integral(ComponentKind::Transform)] = trans_id;
    TransformComponent& trans = compsys->GetTransform(trans_id);
    trans.pos.x = 2.0f;
    trans.pos.y = 2.0f;
    trans.pos.z = 0.0f;
    trans.scale = {0.25f, 0.25f, 0.25f};
    trans.rotation = {};

    ComponentHandle render_comp_id = compsys->MakeRender();
    bear.components[to_integral(ComponentKind::Render)] = render_comp_id;
    RenderComponent& render_comp = compsys->GetRender(render_comp_id);
    render_comp.color = Colors::red;
    render_comp.mesh_type = MeshType::Bear;

    ComponentHandle physics_comp_id = compsys->MakePhysics();
    bear.components[to_integral(ComponentKind::Physics)] = physics_comp_id;
    PhysicsComponent& physics_comp = compsys->GetPhysics(physics_comp_id);
    physics_sys->InitializeRigidBody(physics_comp);
    physics_sys->AddPrimitive(physics_comp, ePrimitiveType::Box);
    static_cast<Box*>(physics_comp.primitive)->SetBox(0.5f, 0.5f, 0.5f);
    physics_comp.rigid_body.SetPosition(trans.pos);
    trans.rotation = physics_comp.rigid_body.GetOrientation();
  }

  duck_id = Get<EntitySys>()->MakeEntity();

  {
    GameEntity& duck = Get<EntitySys>()->GetEntity((duck_id));
    ComponentHandle trans_id = compsys->MakeTransform();
    duck.components[to_integral(ComponentKind::Transform)] = trans_id;
    TransformComponent& trans = compsys->GetTransform(trans_id);
    trans.pos.x = -2.0f;
    trans.pos.y = 2.0f;
    trans.pos.z = 0.0f;
    trans.scale = {0.25f, 0.25f, 0.25f};
    trans.rotation = {};

    ComponentHandle render_comp_id = compsys->MakeRender();
    duck.components[to_integral(ComponentKind::Render)] = render_comp_id;
    RenderComponent& render_comp = compsys->GetRender(render_comp_id);
    render_comp.color = Colors::peach;
    render_comp.mesh_type = MeshType::Duck;

    ComponentHandle physics_comp_id = compsys->MakePhysics();
    duck.components[to_integral(ComponentKind::Physics)] = physics_comp_id;
    PhysicsComponent& physics_comp = compsys->GetPhysics(physics_comp_id);
    physics_sys->InitializeRigidBody(physics_comp);
    physics_sys->AddPrimitive(physics_comp, ePrimitiveType::Box);
    static_cast<Box*>(physics_comp.primitive)->SetBox(0.5f, 0.5f, 0.5f);
    physics_comp.rigid_body.SetPosition(trans.pos);
    trans.rotation = physics_comp.rigid_body.GetOrientation();

    //this is test code, it does nothing so dont worry about it
    /*ComponentHandle bhvr_comp_id = compsys->MakeBehavior();
           obj101_entity.components[to_integral(ComponentKind::Behavior)] = bhvr_comp_id;
            BehaviorComponent& beh_comp = compsys->GetBehavior(bhvr_comp_id);
            beh_comp.type = BHVRType::WINDTUNNEL;*/
  }

  bunny_id = Get<EntitySys>()->MakeEntity();

  {
    GameEntity& bunny = Get<EntitySys>()->GetEntity((bunny_id));
    ComponentHandle trans_id = compsys->MakeTransform();
    bunny.components[to_integral(ComponentKind::Transform)] = trans_id;
    TransformComponent& trans = compsys->GetTransform(trans_id);
    trans.pos.x = 0.0f;
    trans.pos.y = 2.0f;
    trans.pos.z = -2.0f;
    trans.scale = {0.25f, 0.25f, 0.25f};
    trans.rotation = {};

    ComponentHandle render_comp_id = compsys->MakeRender();
    bunny.components[to_integral(ComponentKind::Render)] = render_comp_id;
    RenderComponent& render_comp = compsys->GetRender(render_comp_id);
    render_comp.color = Colors::green;
    render_comp.mesh_type = MeshType::Bunny;

    ComponentHandle physics_comp_id = compsys->MakePhysics();
    bunny.components[to_integral(ComponentKind::Physics)] = physics_comp_id;
    PhysicsComponent& physics_comp = compsys->GetPhysics(physics_comp_id);
    physics_sys->InitializeRigidBody(physics_comp);
    physics_sys->AddPrimitive(physics_comp, ePrimitiveType::Box);
    static_cast<Box*>(physics_comp.primitive)->SetBox(0.5f, 0.5f, 0.5f);
    physics_comp.rigid_body.SetPosition(trans.pos);
    trans.rotation = physics_comp.rigid_body.GetOrientation();
  }

#if 0

        wind_tunnel_id = Get<EntitySys>()->MakeEntity();

        {
            GameEntity& obj102_entity = Get<EntitySys>()->GetEntity((wind_tunnel_id));
            ComponentHandle trans_id = compsys->MakeTransform();
            obj102_entity.components[to_integral(ComponentKind::Transform)] = trans_id;
            TransformComponent& trans = compsys->GetTransform(trans_id);
            trans.pos.x = 0.0f;
            trans.pos.y = 1.0f;
            trans.pos.z = 5.0f;
            trans.scale = { 2.0f, 2.0f, 2.0f };
            trans.rotation = {};

            ComponentHandle render_comp_id = compsys->MakeRender();
            obj102_entity.components[to_integral(ComponentKind::Render)] = render_comp_id;
            RenderComponent& render_comp = compsys->GetRender(render_comp_id);
            render_comp.color = Colors::white;
            render_comp.mesh_type = MeshType::Cube;
        }
#endif

#if 0
        wind_tunnel_physics.rigid_body = world->AddRigidBody();
        wind_tunnel_physics.rigid_body->SetLinearConstraints(constraints);
        wind_tunnel_physics.rigid_body->SetAngularConstraints(constraints);
        wind_tunnel_physics.primitive = world->AddPrimitive(wind_tunnel_physics.rigid_body, ePrimitiveType::Box);
        static_cast<Box*>(wind_tunnel_physics.primitive)->SetBox(2.0f, 2.0f, 2.0f);
#endif

  //wind_tunnel_physics.rigid_body->SetPosition({0.f, 1.f, 5.f});
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
    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize
      | ImGuiWindowFlags_NoNav);

  ImGui::Text("WASD and Space/Shift for camera movement");
  ImGui::Text("Alt+Enter for Fullscreen");

  if (ImGui::CollapsingHeader("Default"))
  {
    if (ImGui::TreeNode("Objects"))
    {
      auto* physics_sys = Get<PhysicsSys>();
      if (ImGui::Button("Default Bear Position"))
      {
        GameEntity& bear = Get<EntitySys>()->GetEntity((bear_id));
        TransformComponent& trans
          = Get<ComponentSys>()->GetTransform(bear.GetComponentHandle(ComponentKind::Transform));
        trans.pos.x = 2.0f;
        trans.pos.y = 2.0f;
        trans.pos.z = 0.0f;
        trans.scale = {0.25f, 0.25f, 0.25f};
        trans.rotation = {};

        PhysicsComponent& physics_comp
          = Get<ComponentSys>()->GetPhysics(bear.GetComponentHandle(ComponentKind::Physics));
        physics_sys->InitializeRigidBody(physics_comp);
        physics_comp.rigid_body.SetPosition(trans.pos);
        trans.rotation = physics_comp.rigid_body.GetOrientation();

        dt = 0.f;
      }

      if (ImGui::Button("Default Duck Position"))
      {
        GameEntity& duck = Get<EntitySys>()->GetEntity((duck_id));
        TransformComponent& trans
          = Get<ComponentSys>()->GetTransform(duck.GetComponentHandle(ComponentKind::Transform));
        trans.pos.x = -2.0f;
        trans.pos.y = 2.0f;
        trans.pos.z = 0.0f;
        trans.scale = {0.25f, 0.25f, 0.25f};
        trans.rotation = {};

        PhysicsComponent& physics_comp
          = Get<ComponentSys>()->GetPhysics(duck.GetComponentHandle(ComponentKind::Physics));
        physics_sys->InitializeRigidBody(physics_comp);
        physics_comp.rigid_body.SetPosition(trans.pos);
        trans.rotation = physics_comp.rigid_body.GetOrientation();

        dt = 0.f;
      }

      if (ImGui::Button("Default Bunny Position"))
      {
        GameEntity& bunny = Get<EntitySys>()->GetEntity((bunny_id));
        TransformComponent& trans
          = Get<ComponentSys>()->GetTransform(bunny.GetComponentHandle(ComponentKind::Transform));
        trans.pos.x = 0.0f;
        trans.pos.y = 2.0f;
        trans.pos.z = -2.0f;
        trans.scale = {0.25f, 0.25f, 0.25f};
        trans.rotation = {};

        PhysicsComponent& physics_comp
          = Get<ComponentSys>()->GetPhysics(bunny.GetComponentHandle(ComponentKind::Physics));
        physics_sys->InitializeRigidBody(physics_comp);
        physics_comp.rigid_body.SetPosition(trans.pos);
        trans.rotation = physics_comp.rigid_body.GetOrientation();

        dt = 0.f;
      }

      ImGui::TreePop();
    }
  }

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

    auto handle = Get<EntitySys>()->GetEntity(bear_id).GetComponentHandle(ComponentKind::Physics);

    PhysicsComponent& bear_physics = Get<ComponentSys>()->GetPhysics(
      Get<EntitySys>()->GetEntity(bear_id).GetComponentHandle(ComponentKind::Physics));
    PhysicsComponent& duck_physics = Get<ComponentSys>()->GetPhysics(
      Get<EntitySys>()->GetEntity(duck_id).GetComponentHandle(ComponentKind::Physics));
    PhysicsComponent& bunny_physics = Get<ComponentSys>()->GetPhysics(
      Get<EntitySys>()->GetEntity(bunny_id).GetComponentHandle(ComponentKind::Physics));
    auto& bear_trans = Get<ComponentSys>()->GetTransform(
      Get<EntitySys>()->GetEntity(bear_id).GetComponentHandle(ComponentKind::Transform));

    auto& duck_trans = Get<ComponentSys>()->GetTransform(
      Get<EntitySys>()->GetEntity(duck_id).GetComponentHandle(ComponentKind::Transform));

    auto& bunny_trans = Get<ComponentSys>()->GetTransform(
      Get<EntitySys>()->GetEntity(bunny_id).GetComponentHandle(ComponentKind::Transform));

    auto& bear_pos = bear_trans.pos;   //bear_physics.rigid_body.GetPosition();
    auto& duck_pos = duck_trans.pos;   //duck_physics.rigid_body.GetPosition();
    auto& bunny_pos = bunny_trans.pos; //bunny_physics.rigid_body.GetPosition();

    float constexpr G = 9.81f;
    //TODO: add collider to ground plane and then remove these lines
    LockYRelToTarget(bear_pos, {0.f, 0.f, 0.f}, -.25f);
    LockYRelToTarget(duck_pos, {0.f, 0.f, 0.f}, -.25f);
    LockYRelToTarget(bunny_pos, {0.f, 0.f, 0.f}, -.25f);

    bear_physics.rigid_body.SetPosition(bear_pos);
    duck_physics.rigid_body.SetPosition(duck_pos);
    bunny_physics.rigid_body.SetPosition(bunny_pos);

    //apply gravity
    bear_physics.rigid_body.ApplyForceCentroid({0.f, -G, 0.f});
    duck_physics.rigid_body.ApplyForceCentroid({0.f, -G, 0.f});
    bunny_physics.rigid_body.ApplyForceCentroid({0.f, -G, 0.f});

    dx::XMFLOAT3 bear_velocity;
    bear_velocity.x = -1.0f * (input->KeyHeld(SDLK_LEFT) - input->KeyHeld(SDLK_RIGHT));
    bear_velocity.y = 0.0f;
    bear_velocity.z = -1.0f * (input->KeyHeld(SDLK_UP) - input->KeyHeld(SDLK_DOWN));

    bear_physics.rigid_body.ApplyForceCentroid(bear_velocity);

    bool jumpPlease = input->KeyPressed(SDLK_j);
    auto cam_pos = camera.GetPosition();
    //enemy movement
    SimpleMove(bear_physics.rigid_body, bear_pos, cam_pos, 1.05f);
    SimpleMove(duck_physics.rigid_body, duck_pos, cam_pos, 0.95f);
    SimpleMove(bunny_physics.rigid_body, bunny_pos, cam_pos, 0.505f);

    FacePos(bear_trans, cam_pos);  //, false, true, false);
    FacePos(duck_trans, cam_pos);  //, false, true, false);
    FacePos(bunny_trans, cam_pos); //, false, true, false);
    //make them slightly more interesting by having them jump
    RandomJump(bear_physics.rigid_body, bear_pos, jumpPlease ? 100.f : 0.2f, 15.f * G);
    RandomJump(duck_physics.rigid_body, duck_pos, jumpPlease ? 100.f : 0.7f, 25.f * G);
    BunnyHop(bunny_physics.rigid_body, bunny_pos, 35.f * G);
  }
}

void TestScene::End() {}

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
