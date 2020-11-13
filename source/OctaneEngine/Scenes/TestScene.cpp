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
#include <OctaneEngine/Scenes/TestScene.h>

#include <OctaneEngine/Engine.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/FramerateController.h>
#include <OctaneEngine/Graphics/CameraSys.h>
#include <OctaneEngine/Graphics/RenderSys.h>
#include <OctaneEngine/ImGuiSys.h>
#include <OctaneEngine/InputHandler.h>
#include <OctaneEngine/WindowManager.h>
#include <OctaneEngine/behaviors/BearBehavior.h>
#include <OctaneEngine/behaviors/PlaneBehavior.h>
#include <OctaneEngine/behaviors/EnemySpawner.h>

#include <EASTL/optional.h>
#include <EASTL/string.h>
#include <EASTL/string_view.h>
#include <imgui.h>
#include <iostream>
#define MAGIC_ENUM_USING_ALIAS_OPTIONAL \
  template<typename T>                  \
  using optional = eastl::optional<T>;
#define MAGIC_ENUM_USING_ALIAS_STRING      using string = eastl::string;
#define MAGIC_ENUM_USING_ALIAS_STRING_VIEW using string_view = eastl::string_view;
#include <magic_enum.hpp>

#include <OctaneEngine/NBTReader.h>
#include <OctaneEngine/Trace.h>

#include <OctaneEngine/Physics/Box.h>
#include <OctaneEngine/Physics/PhysicsSys.h>

#include <OctaneEngine/AudioPlayer.h>
#include <OctaneEngine/PlayerMovementControllerSys.h>
#include <OctaneEngine/TransformHelpers.h>
#include <OctaneEngine/behaviors/WindTunnelBhv.h>

// define to use actual player entity instead of separate camera movement
#define USE_PLAYER_ENTITY

namespace dx = DirectX;

// static for testing
AkGameObjectID spawner;

namespace
{

Octane::EntityID spawner_id;
Octane::EntityID wind_tunnel_id;
Octane::EntityID crossbow_id;
const dx::XMFLOAT3 PHYSICS_CONSTRAINTS = {1.0f, 1.0f, 1.0f};
const dx::XMFLOAT3 WINDTUNNELFORCE = {-100.f, 30.f, 0.f};
Octane::EnemyDestroyed enemy_destroyed_func;
float shootDelay = 0.01f;
float shootTimer = shootDelay;
bool can_shoot = true;
} // namespace

namespace Octane
{

TestScene::TestScene(SceneSys* parent) : IScene(parent) {}

void TestScene::Load()
{
  enemy_destroyed_func.enemiesSpawned = 0;
  enemy_destroyed_func.score = 0;
  auto* entsys = Get<EntitySys>();
  auto* compsys = Get<ComponentSys>();
  auto* physics_sys = Get<PhysicsSys>();

  auto create_object = [=](dx::XMFLOAT3 pos, dx::XMFLOAT3 scale, Color color, MeshType mesh_type = MeshType::Cube) {
    // todo: custom entity_id / component_id types with overridden operator*, because this is way too much boilerplate
    EntityID const ent_id = entsys->MakeEntity();
    GameEntity& game_entity = entsys->GetEntity((ent_id));
    ComponentHandle const trans_id = compsys->MakeTransform();
    game_entity.components[to_integral(ComponentKind::Transform)] = trans_id;
    TransformComponent& trans = compsys->GetTransform(trans_id);
    trans.pos = pos;
    trans.scale = scale;
    trans.rotation = {0, 0, 0, 1};
    ComponentHandle const render_id = compsys->MakeRender();
    game_entity.components[to_integral(ComponentKind::Render)] = render_id;
    RenderComponent& render_component = compsys->GetRender(render_id);
    render_component.color = color;
    render_component.mesh_type = mesh_type;

    //ComponentHandle physics_comp_id = compsys->MakePhysics();
    //game_entity.components[to_integral(ComponentKind::Physics)] = physics_comp_id;
    //PhysicsComponent& physics_comp = compsys->GetPhysics(physics_comp_id);
    //physics_sys->InitializeRigidBody(physics_comp);
    //physics_sys->AddPrimitive(physics_comp, ePrimitiveType::Box);
    //static_cast<Box*>(physics_comp.primitive)->SetBox(2.0f * scale.x, 2.0f * scale.y, 2.0f * scale.z);
    //physics_comp.rigid_body.SetPosition(trans.pos);
    //trans.rotation = physics_comp.rigid_body.GetOrientation();
    //physics_comp.rigid_body.SetStatic();
  };

  {
    Trace::Log(DEBUG, "Loading entities.\n");
    EntitySys& entity_sys = *Get<EntitySys>();
    ComponentSys& component_sys = *Get<ComponentSys>();

    Trace::Log(DEBUG, "Loading entities.\n");
    NBTReader nbt_reader {"assets/maps/level1.nbt"};
    // for every object, read it in
    if (nbt_reader.OpenList("Entities"))
    {
      const int entity_list_size = nbt_reader.ListSize();
      for (int i = 0; i < entity_list_size; ++i)
      {
        if (nbt_reader.OpenCompound(""))
        {
          EntityID const ent_id = entity_sys.MakeEntity();
          GameEntity& ent = entity_sys.GetEntity(ent_id);

          for (auto component_type : magic_enum::enum_values<ComponentKind>())
          {
            if (nbt_reader.OpenCompound(magic_enum::enum_name(component_type)))
            {
              switch (component_type)
              {
              case ComponentKind::Render:
              {
                ComponentHandle const render_id = component_sys.MakeRender();
                ent.components[to_integral(ComponentKind::Render)] = render_id;
                RenderComponent& render_component = component_sys.GetRender(render_id);
                render_component.color = nbt_reader.Read<Color>("Color");
                render_component.mesh_type = nbt_reader.Read<MeshType>("Mesh");
              }
              break;
              case ComponentKind::Transform:
              {
                ComponentHandle const trans_id = component_sys.MakeTransform();
                ent.components[to_integral(ComponentKind::Transform)] = trans_id;
                TransformComponent& trans = component_sys.GetTransform(trans_id);
                trans.pos = nbt_reader.Read<DirectX::XMFLOAT3>("Pos");
                trans.scale = nbt_reader.Read<DirectX::XMFLOAT3>("Scale");
                trans.rotation = nbt_reader.Read<DirectX::XMFLOAT4>("Rotation");
              }
              break;
              default: break;
              }
              nbt_reader.CloseCompound();
            }
          }
          nbt_reader.CloseCompound();
        }
      }
      nbt_reader.CloseList();
    }

    spawner = 2;
    AudioPlayer::Register_Object(spawner, "spawner");
  }

   
// this WAS commented-out because of behavior sys bugs
// now it works flawlessly and it is the enemy spawner
#if 1
  {
    auto spawner_id = Get<EntitySys>()->MakeEntity();
    GameEntity& enemy_spawner = Get<EntitySys>()->GetEntity((spawner_id));
    ComponentHandle trans_id = compsys->MakeTransform();
    enemy_spawner.components[to_integral(ComponentKind::Transform)] = trans_id;
    TransformComponent& trans = compsys->GetTransform(trans_id);
    trans.pos ={0.f,0.f,0.f};
    ComponentHandle behavior_comp_id = compsys->MakeBehavior(BHVRType::ENEMYSPAWNER);
    enemy_spawner.components[to_integral(ComponentKind::Behavior)] = behavior_comp_id;
    BehaviorComponent& behavior_comp = compsys->GetBehavior(behavior_comp_id);
    behavior_comp.type = BHVRType::ENEMYSPAWNER;
    static_cast<EnemySpawner*>(behavior_comp.behavior)->SetEnemyDestroyedFunc(enemy_destroyed_func);
  }
#endif

  crossbow_id = Get<EntitySys>()->MakeEntity();
  {
    GameEntity& crossbow = Get<EntitySys>()->GetEntity((crossbow_id));
    ComponentHandle trans_id = compsys->MakeTransform();
    crossbow.components[to_integral(ComponentKind::Transform)] = trans_id;
    TransformComponent& trans = compsys->GetTransform(trans_id);
    trans.pos.x = 0.0f;
    trans.pos.y = 0.0f;
    trans.pos.z = 0.0f;
    trans.scale = {0.1f, 0.1f, 0.1f};
    trans.rotation = {};

    ComponentHandle render_comp_id = compsys->MakeRender();
    crossbow.components[to_integral(ComponentKind::Render)] = render_comp_id;
    RenderComponent& render_comp = compsys->GetRender(render_comp_id);
    render_comp.color = Colors::cerulean;
    render_comp.mesh_type = MeshType::Crossbow;
  }

#ifdef USE_PLAYER_ENTITY
  Octane::EntityID player_id = Get<EntitySys>()->MakeEntity();

  {
    GameEntity& player = Get<EntitySys>()->GetEntity(player_id);
    ComponentHandle trans_id = compsys->MakeTransform();
    player.components[to_integral(ComponentKind::Transform)] = trans_id;
    TransformComponent& trans = compsys->GetTransform(trans_id);
    trans.pos = {10.0f, 0.5f, 7.0f};
    trans.scale = {1.0f, 1.0f, 1.0f};
    trans.rotation = {};

    ComponentHandle physics_comp_id = compsys->MakePhysics();
    player.components[to_integral(ComponentKind::Physics)] = physics_comp_id;
    PhysicsComponent& physics_comp = compsys->GetPhysics(physics_comp_id);
    physics_sys->InitializeRigidBody(physics_comp);
    physics_sys->AddPrimitive(physics_comp, ePrimitiveType::Box);
    static_cast<Box*>(physics_comp.primitive)->SetBox(01.5f, 01.5f, 01.5f);
    physics_comp.rigid_body.SetPosition(trans.pos);
    physics_comp.rigid_body.SetStatic(); // should stop other objects from applying physics to player
    trans.rotation = physics_comp.rigid_body.GetOrientation();

    Get<EntitySys>()->SetPlayerID(player_id);
  }
#endif

#if 1

  wind_tunnel_id = Get<EntitySys>()->MakeEntity();

  {
    GameEntity& obj102_entity = Get<EntitySys>()->GetEntity((wind_tunnel_id));
    ComponentHandle trans_id = compsys->MakeTransform();
    obj102_entity.components[to_integral(ComponentKind::Transform)] = trans_id;
    TransformComponent& trans = compsys->GetTransform(trans_id);
    trans.pos.x = 10.0f;
    trans.pos.y = 1.0f;
    trans.pos.z = 3.0f;
    trans.scale = {2.0f, 2.0f, 2.0f};
    trans.rotation = {};

    ComponentHandle render_comp_id = compsys->MakeRender();
    obj102_entity.components[to_integral(ComponentKind::Render)] = render_comp_id;
    RenderComponent& render_comp = compsys->GetRender(render_comp_id);
    render_comp.color = Colors::red;
    render_comp.mesh_type = MeshType::Cube;
    render_comp.render_type = RenderType::Wireframe;

    ComponentHandle physics_comp_id = compsys->MakePhysics();
    obj102_entity.components[to_integral(ComponentKind::Physics)] = physics_comp_id;
    PhysicsComponent& physics_comp = compsys->GetPhysics(physics_comp_id);
    physics_sys->InitializeRigidBody(physics_comp);
    physics_sys->AddPrimitive(physics_comp, ePrimitiveType::Box);
    static_cast<Box*>(physics_comp.primitive)->SetBox(4.0f, 4.0f, 4.0f);
    physics_comp.rigid_body.SetPosition(trans.pos);
    physics_comp.rigid_body.SetStatic();
    //physics_comp.rigid_body.SetGhost(true);
    trans.rotation = physics_comp.rigid_body.GetOrientation();

    ComponentHandle bhvr_comp_id = compsys->MakeBehavior(BHVRType::WINDTUNNEL);
    obj102_entity.components[to_integral(ComponentKind::Behavior)] = bhvr_comp_id;
    BehaviorComponent& beh_comp = compsys->GetBehavior(bhvr_comp_id);
    beh_comp.force = WINDTUNNELFORCE;
  }

#endif
}

void TestScene::Start()
{
  esc_menu = false;
  demo_window_open = false;
  Get<RenderSys>()->SetClearColor(Colors::db32[19]);
  SDL_SetRelativeMouseMode(SDL_TRUE);
  fov = Get<CameraSys>()->GetFOV();
}

void TestScene::Update(float dt)
{
  auto* entsys = Get<EntitySys>();
  auto* compsys = Get<ComponentSys>();
  auto* physics_sys = Get<PhysicsSys>();


  auto create_plane = [=](dx::XMFLOAT3 pos)
  {
    auto id = entsys->CreateEntity(pos, {.05f, .05f, .05f}, {});
    GameEntity& plane = Get<EntitySys>()->GetEntity((id));
    entsys->AddPhysics(id, ePrimitiveType::Box, {0.1f, 0.1f, 0.1f});
    entsys->AddRenderComp(id, Colors::db32[rand() % 32], MeshType::PaperPlane);
    entsys->AddBehavior(id, BHVRType::PLANE);
  };

  ImGui::Begin(
    "Instructions Window",
    NULL,
    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize
      | ImGuiWindowFlags_NoNav);

  ImGui::Text("WASD to walk around");
  ImGui::Text("Spacebar to jump, Shift to sneak");
  ImGui::Text("Hold space to hover in the air");
  ImGui::Text("Left mouse button to shoot, Right mouse button to zoom");
  ImGui::Text("E to spawn a wind tunnel that go to where the plsyer is facing");
  ImGui::Text("G to spawn a wind tunnel that locks onto the closest enemy");
  ImGui::Text("The wireframe box is wind, it will carry your projectiles towards enemies.");
  ImGui::Text("F to switch weapons.");
  ImGui::Text("Alt+Enter for Fullscreen\n");

  ImGui::Text("Score: %d", enemy_destroyed_func.score);
  ImGui::Text("High Score: %d", enemy_destroyed_func.highScore);

  ImGui::End();

  ImGui::SetWindowPos("Instructions Window", {0.0f, ImGui::GetWindowSize().y * 0.5f});

  Health& player_HP = Get<PlayerMovementControllerSys>()->GetHealth();
  for (int i = 0; i < player_HP.GetMaxHP(); ++i)
  {
    ImVec2 hp_size = {90.0f, 90.0f};
    ImVec2 hp_spacing = {10.0f, 20.0f};
    Color fill = (i < player_HP.GetCurrentHP()) ? Colors::db32[27] : Colors::db32[25];
    ImGui::GetOverlayDrawList()->AddRectFilled(
      {hp_spacing.x + (hp_spacing.x + hp_size.x) * i, hp_spacing.y},
      {(hp_spacing.x + hp_size.x) * (i + 1), hp_spacing.y + hp_size.y},
      ImGui::GetColorU32({fill.r, fill.g, fill.b, 1.0f}));
  }

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

  auto* pmhandler = Get<PlayerMovementControllerSys>();

  if (esc_menu)
  {
    Get<FramerateController>()->Pause();
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
      Octane::AudioPlayer::Play_Event(AK::EVENTS::PLAY_BUTTONSELECT);
      esc_menu = false;
      SDL_SetRelativeMouseMode(SDL_TRUE);
    }
    if (ImGui::CollapsingHeader("Option"))
    {
      ImGui::Text("Mouse Sensitivity: %d", mouse_sens);
      if (ImGui::Button("+ Mouse Sensitivity"))
      {
        pmhandler->IncreaseMouseSense();
        mouse_sens += 5;
      }
      if (ImGui::Button("- Mouse Sensitivity"))
      {
        pmhandler->DecreaseMouseSense();
        mouse_sens -= 5;
      }

    }
    if (ImGui::Button("Main Menu"))
    {
      Octane::AudioPlayer::Play_Event(AK::EVENTS::PLAY_BUTTONBACK);
      Get<SceneSys>()->SetNextScene(SceneE::MenuScene);
    }
    if (ImGui::Button("Quit"))
    {
      Octane::AudioPlayer::Play_Event(AK::EVENTS::PLAY_BUTTONBACK);
      Get<SceneSys>()->Quit();
    }

    ImGui::End();
  }
  else
  {
    Get<FramerateController>()->Unpause();

    {
      GameEntity& wind_tunnel_entity = entsys->GetEntity(wind_tunnel_id);
      ComponentHandle wind_behavior = wind_tunnel_entity.GetComponentHandle(ComponentKind::Behavior);
      BehaviorComponent& beh_comp = compsys->GetBehavior(wind_behavior);
      ComponentHandle wind_transform = wind_tunnel_entity.GetComponentHandle(ComponentKind::Transform);
      TransformComponent& wind_transform_comp = compsys->GetTransform(wind_transform);

      float closest_dist = FLT_MAX;

      dx::XMVECTOR wind_pos = dx::XMLoadFloat3(&wind_transform_comp.pos);
      dx::XMVECTOR closest_plane = wind_pos;
      for (auto it = entsys->EntitiesBegin(); it != entsys->EntitiesEnd(); ++it)
      {
        if (it->HasComponent(ComponentKind::Behavior))
        {
          auto other = it->GetComponentHandle(ComponentKind::Behavior);
          const auto& othbeh = Get<ComponentSys>()->GetBehavior(other);

          if (othbeh.type == BHVRType::PLANE)
          {
            ComponentHandle plane_transform = it->GetComponentHandle(ComponentKind::Transform);
            TransformComponent& plane_transform_comp = compsys->GetTransform(plane_transform);
            dx::XMVECTOR plane_pos = dx::XMLoadFloat3(&plane_transform_comp.pos);
            dx::XMVECTOR wind_to_plane = dx::XMVectorSubtract(plane_pos, wind_pos);
            float const distance = dx::XMVector3LengthSq(wind_to_plane).m128_f32[0];
            if (distance < closest_dist)
            {
              closest_plane = plane_pos;
              closest_dist = distance;
            }
          }
        }
      }

      dx::XMFLOAT3 wind_dir = WINDTUNNELFORCE;
      closest_dist = FLT_MAX;

      for (auto it = entsys->EntitiesBegin(); it != entsys->EntitiesEnd(); ++it)
      {
        if (it->HasComponent(ComponentKind::Behavior))
        {
          auto other = it->GetComponentHandle(ComponentKind::Behavior);
          const auto& othbeh = Get<ComponentSys>()->GetBehavior(other);

          if (othbeh.type == BHVRType::BEAR)
          {
            ComponentHandle enemy_transform = it->GetComponentHandle(ComponentKind::Transform);
            TransformComponent& enemy_transform_comp = compsys->GetTransform(enemy_transform);
            dx::XMVECTOR plane_to_enemy
              = dx::XMVectorSubtract(dx::XMLoadFloat3(&enemy_transform_comp.pos), closest_plane);
            float const distance = dx::XMVector3LengthSq(plane_to_enemy).m128_f32[0];
            if (distance < closest_dist)
            {
              float wind_strength = 20.0f;
              dx::XMVECTOR plane_flight = dx::XMVectorScale(dx::XMVector3Normalize(plane_to_enemy), wind_strength);
              dx::XMStoreFloat3(&wind_dir, dx::XMVectorAdd(plane_flight, {0, 9.81f * 2.0f / (wind_strength), 0}));
              closest_dist = distance;
            }
          }
        }
      }
      beh_comp.force = wind_dir;
    }

    auto* input = Get<InputHandler>();
    auto& camera = Get<CameraSys>()->GetFPSCamera();

    auto& crossbow_trans = Get<ComponentSys>()->GetTransform(
      Get<EntitySys>()->GetEntity(crossbow_id).GetComponentHandle(ComponentKind::Transform));

    auto cam_pos = camera.GetPosition();

    //crossbow placement and rotation
    PlaceRelativeTo(crossbow_trans, 0.25f, cam_pos, camera.GetInverseOrientation(), camera.GetViewDirection());

    //shoot paper airplanes
    if (input->MouseButtonPressed(InputHandler::MouseButton::LEFT) && can_shoot)
    {
      Octane::AudioPlayer::Play_Event(AK::EVENTS::PLAY_CROSSBOW);
      can_shoot = false;
      create_plane(crossbow_trans.pos);
    }
    
    if (input->KeyPressed(SDLK_f))
    {
      auto& weapon = entsys->GetEntity(crossbow_id);
      auto& weapon_render = compsys->GetRender(weapon.GetComponentHandle(ComponentKind::Render));

      switch (weapon_render.mesh_type)
      {
      case MeshType::Crossbow:
        weapon_render.mesh_type = MeshType::Slingshot;
        break;
      case MeshType::Slingshot:
        weapon_render.mesh_type = MeshType::Crossbow;
        break;
      }
    }

    if (enemy_destroyed_func.spawnedWave)
    {
      AudioPlayer::Set_Position(spawner, {0.f,1.f,0.f});
      AudioPlayer::Play_Event(AK::EVENTS::ENEMY_SPAWN, spawner);
    }

    shootTimer += dt;
    if (shootTimer >= shootDelay)
    {
      can_shoot = true;
      shootTimer = 0.0f;
    }
  }
}

void TestScene::End() {}

void TestScene::Unload()
{
  SDL_SetRelativeMouseMode(SDL_FALSE);
  auto* entsys = Get<EntitySys>();
  auto* compsys = Get<ComponentSys>();

  entsys->FreeAllEntities();

  //delete blue_bear_physics.rigid_body;
  //delete blue_bear_physics.primitive;
  //delete red_bear_physics.rigid_body;
  //delete red_bear_physics.primitive;
  AudioPlayer::Unregister_Object(spawner);
}

SceneE TestScene::GetEnum() const
{
  return SceneE::TestScene;
}

} // namespace Octane
