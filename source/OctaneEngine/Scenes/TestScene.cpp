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

#include <OctaneEngine/AudioPlayer.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/FramerateController.h>
#include <OctaneEngine/Graphics/CameraSys.h>
#include <OctaneEngine/Graphics/RenderSys.h>
#include <OctaneEngine/ImGuiSys.h>
#include <OctaneEngine/InputHandler.h>
#include <OctaneEngine/NBTReader.h>
#include <OctaneEngine/Physics/PhysicsSys.h>
#include <OctaneEngine/PlayerMovementControllerSys.h>
#include <OctaneEngine/Trace.h>
#include <OctaneEngine/TransformHelpers.h>
#include <OctaneEngine/WindowManager.h>
#include <OctaneEngine/behaviors/BearBehavior.h>
#include <OctaneEngine/behaviors/EnemySpawner.h>
#include <OctaneEngine/behaviors/PlaneBehavior.h>
#include <OctaneEngine/behaviors/WindTunnelBhv.h>

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

// define to use actual player entity instead of separate camera movement
#define USE_PLAYER_ENTITY

namespace dx = DirectX;

// static for testing
AkGameObjectID spawner;

namespace
{

Octane::EntityID spawner_id;
//Octane::EntityID wind_tunnel_id;
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
  enemy_destroyed_func.wave = 1;
  enemy_destroyed_func.enemiesKilled = 0;
  enemy_destroyed_func.spawnedWave = true;
  auto* entsys = Get<EntitySys>();
  auto* compsys = Get<ComponentSys>();
  auto* physics_sys = Get<PhysicsSys>();

  auto create_object = [=](dx::XMFLOAT3 pos, dx::XMFLOAT3 scale, Color color, Mesh_Key mesh_type = Mesh_Key {"Cube"}) {
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
    render_component.material.diffuse = color;
    render_component.mesh_type = mesh_type;
  };

  {
    Trace::Log(DEBUG, "Loading entities.\n");
    EntitySys& entity_sys = *Get<EntitySys>();
    ComponentSys& component_sys = *Get<ComponentSys>();

    NBTReader nbt_reader {"assets/maps/level1.nbt"};
    // for every object, read it in
    // for every object, read it in
    if (nbt_reader.OpenList("Entities"))
    {
      const int entity_list_size = nbt_reader.ListSize();
      for (int i = 0; i < entity_list_size; ++i)
      {
        if (nbt_reader.OpenCompound(""))
        {
          EntityID const ent_id = entity_sys.MakeEntity();
          GameEntity& ent = entity_sys.GetEntity((ent_id));

          ComponentHandle const metadata_id = component_sys.MakeMetadata();
          ent.components[to_integral(ComponentKind::Metadata)] = metadata_id;
          MetadataComponent& metadata_component = component_sys.GetMetadata(metadata_id);
          metadata_component.name = nbt_reader.MaybeReadString("Name").value_or("");

          for (auto component_type : magic_enum::enum_values<ComponentKind>())
          {
            if (nbt_reader.OpenCompound(magic_enum::enum_name(component_type)))
            {
              switch (component_type)
              {
              case ComponentKind::Render:
              {
                auto const& meshnames = Get<MeshSys>()->MeshNames();
                RenderComponent& render_component = entity_sys.AddRenderComp(
                  ent_id,
                  nbt_reader.Read<Color>("Color"),
                  *std::find(meshnames.begin(), meshnames.end(), nbt_reader.Read<Mesh_Key>("Mesh")));
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
              case ComponentKind::Physics:
              {
                TransformComponent& trans
                  = component_sys.GetTransform(ent.GetComponentHandle(ComponentKind::Transform));
                float const mass = nbt_reader.ReadFloat("Mass");
                ComponentHandle const phys_id = component_sys.MakePhysicsBox(trans, trans.scale, mass);
                ent.GetComponentHandle(ComponentKind::Physics) = phys_id;
                PhysicsComponent& physics_component = component_sys.GetPhysics(phys_id);
                physics_component.SetRotation(trans.rotation);
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
    trans.pos = {0.f, 0.f, 0.f};
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
    trans.pos = {0.5f, -0.9f, .0f};
    trans.scale = {1.63f, 1.63f, 1.63f};
    trans.rotation = {-0.0605163462f, -0.753363013f, -0.113361359f, -0.644904613f};
    entsys->AddRenderComp(crossbow_id, Colors::cerulean, Mesh_Key {"Sniper1"});
    auto& render_comp = compsys->GetRender(crossbow.GetComponentHandle(ComponentKind::Render));
    render_comp.shader_type = ShaderType::PhongUI;
    //render_comp.render_type = RenderType::Invisible;
  }

#ifdef USE_PLAYER_ENTITY
  EntityID player_id = Get<EntitySys>()->MakeEntity();

  {
    GameEntity& player = Get<EntitySys>()->GetEntity(player_id);
    ComponentHandle trans_id = compsys->MakeTransform();
    player.components[to_integral(ComponentKind::Transform)] = trans_id;
    TransformComponent& trans = compsys->GetTransform(trans_id);
    trans.pos = {10.0f, 0.5f, 7.0f};
    trans.scale = {1.0f, 1.0f, 1.0f};

    // for some reason setting mass to 0 (static) makes player unable to move
    // will need to add support for kinematic objects later
    ComponentHandle physics_comp_id = compsys->MakePhysicsBox(trans, {0.5f, 0.5f, 0.5f}, 1);
    player.components[to_integral(ComponentKind::Physics)] = physics_comp_id;
    PhysicsComponent& physics_comp = compsys->GetPhysics(physics_comp_id);

    Get<EntitySys>()->SetPlayerID(player_id);
  }
#endif

#if 1

  /* wind_tunnel_id = Get<EntitySys>()->MakeEntity();
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
    render_comp.material.diffuse = Colors::red;
    render_comp.mesh_type = Mesh_Key{"Cube"};
    render_comp.render_type = RenderType::Wireframe;

    ComponentHandle physics_comp_id = compsys->MakePhysicsBox(trans, {4.0f, 4.0f, 4.0f}, 0.0f, true);
    obj102_entity.components[to_integral(ComponentKind::Physics)] = physics_comp_id;
    PhysicsComponent& physics_comp = compsys->GetPhysics(physics_comp_id);

    ComponentHandle bhvr_comp_id = compsys->MakeBehavior(BHVRType::WINDTUNNEL);
    obj102_entity.components[to_integral(ComponentKind::Behavior)] = bhvr_comp_id;
    BehaviorComponent& beh_comp = compsys->GetBehavior(bhvr_comp_id);
    beh_comp.force = WINDTUNNELFORCE;
  }*/

#endif

  //Gun Crosshair
  {
    auto crosshair = entsys->CreateEntity({0.f, 0.f, 0.f}, {0.05f, 0.05f, 0.05f}, {0.f, 0.f, 0.f, 1.f});
    auto& render_comp = entsys->AddRenderComp(crosshair, Colors::red, Mesh_Key {"Crosshair1"});
    render_comp.shader_type = ShaderType::UI;
  }

#if 0 //texturing examples
  { //example 1: Texture but don't color
    auto textured1 = entsys->CreateEntity({0.f, 0.f, 0.f}, {1.f, 1.f, 1.f}, {0.f, 0.f, 0.f, 0.f});
    auto& render_comp
      = entsys->AddRenderComp(textured1, Colors::red, Mesh_Key {"BeachBall"}); //to texture, use AddRenderComp()
    render_comp.shader_type = ShaderType::Phong;
    render_comp.material.tint = false; //disable coloring of the object, only take texture into account
  }

  { //example 2: Texture and color
    auto textured2 = entsys->CreateEntity({5.f, 0.f, 0.f}, {1.f, 1.f, 1.f}, {0.f, 0.f, 0.f, 0.f});
    auto& render_comp
      = entsys->AddRenderComp(textured2, Colors::red, Mesh_Key {"BeachBall"}); //to texture, use AddRenderComp()
    render_comp.shader_type = ShaderType::Phong;
    render_comp.material.tint = true; //enable coloring of object on top of its texture (additive)
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

  auto create_plane = [=](dx::XMFLOAT3 pos) {
    auto id = entsys->CreateEntity(pos, {.05f, .05f, .05f}, {});
    GameEntity& plane = Get<EntitySys>()->GetEntity((id));

    auto const& transform = Get<ComponentSys>()->GetTransform(plane.GetComponentHandle(ComponentKind::Transform));
    ComponentHandle physcom = Get<ComponentSys>()->MakePhysicsBox(transform, {0.1f, 0.1f, 0.1f}, 0.1f);
    plane.components[to_integral(ComponentKind::Physics)] = physcom;

    entsys->AddRenderComp(id, Colors::db32[rand() % 32], "PaperPlane");
    entsys->AddBehavior(id, BHVRType::PLANE);
  };

  // For playing hover
  if (ImGui::IsAnyItemHovered())
  {
    AudioPlayer::Play_Once(AK::EVENTS::PLAY_BUTTONHOVER);
  }
  else
  {
    AudioPlayer::Reset_Hover();
  }

  ImGui::Begin(
    "Instructions Window",
    NULL,
    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize
      | ImGuiWindowFlags_NoNav);

  //ImGui::Text("WASD to walk around");
  //ImGui::Text("Spacebar to jump, Shift to sneak");
  ////ImGui::Text("Hold space to hover in the air");
  //ImGui::Text("Left mouse button to shoot, Right mouse button to zoom");
  //ImGui::Text("E to spawn a wind tunnel that go to where the player is facing");
  //ImGui::Text("G to spawn a wind tunnel that locks onto the closest enemy");
  //ImGui::Text("The wireframe box is wind, it will carry your projectiles towards enemies.");
  //ImGui::Text("F to switch weapons.");
  //ImGui::Text("Rifle can shoot airplane with auto mode");
  //ImGui::Text("****************************************************************");
  //ImGui::Text("**************          F7 for Fullscreen        ***************");
  //ImGui::Text("****************************************************************");
  //ImGui::Text("");
  ImGui::Text("Score: %d", enemy_destroyed_func.score);
  ImGui::Text("High Score: %d", enemy_destroyed_func.highScore);
  ImGui::Text("Wave: %d", enemy_destroyed_func.wave);
  ImGui::Text("Highest wave achieved: %d", enemy_destroyed_func.highestWave);
  ImGui::Text("Enemies remaining: %d", enemy_destroyed_func.enemiesLeft);

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

  #if DEBUG

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

  #endif

  auto* pmhandler = Get<PlayerMovementControllerSys>();

  if (esc_menu)
  {
    Get<FramerateController>()->Pause();
    SDL_SetRelativeMouseMode(SDL_FALSE);

    ImGui::Begin(
      "Pause Menu",
      NULL,
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
    {
      WindowManager* window = Get<WindowManager>();
      ImGui::SetWindowPos("Pause Menu", ImVec2(0.5f * window->GetWidth(), 0.5f * window->GetHeight()));
    }

    if (ImGui::Button("Resume"))
    {
      AudioPlayer::Play_Event(AK::EVENTS::PLAY_BUTTONSELECT);
      esc_menu = false;
      SDL_SetRelativeMouseMode(SDL_TRUE);
    }
    if (ImGui::CollapsingHeader("Option"))
    {
      //AudioPlayer::Play_Event(AK::EVENTS::PLAY_BUTTONSELECT);
      //ImGui::Text("Mouse Sensitivity: %d", pmhandler->ShowMouseSense());
      if (ImGui::Button("+ Mouse Sensitivity"))
      {
        AudioPlayer::Play_Event(AK::EVENTS::PLAY_BUTTONSELECT);
        pmhandler->IncreaseMouseSense();
        mouse_sens += 5;
      }
      if (ImGui::Button("- Mouse Sensitivity"))
      {
        AudioPlayer::Play_Event(AK::EVENTS::PLAY_BUTTONSELECT);
        pmhandler->DecreaseMouseSense();
        mouse_sens -= 5;
      }
    }
    if (ImGui::CollapsingHeader("How to Play"))
    {
      ImGui::Text("WASD to walk around");
      ImGui::Text("Spacebar to jump, Shift to sneak");
      //ImGui::Text("Hold space to hover in the air");
      ImGui::Text("Left mouse button to shoot, Right mouse button to zoom");
      ImGui::Text("E to spawn a wind tunnel that go to where the player is facing");
      ImGui::Text("G to spawn a wind tunnel that locks onto the closest enemy");
      ImGui::Text("The wireframe box is wind, it will carry your projectiles towards enemies.");
      ImGui::Text("F to switch weapons.");
      ImGui::Text("Rifle can shoot airplane with auto mode");
      ImGui::Text("****************************************************************");
      ImGui::Text("**************          F7 for Fullscreen        ***************");
      ImGui::Text("****************************************************************");
      /*ImGui::Text("");
      ImGui::Text("Score: %d", enemy_destroyed_func.score);
      ImGui::Text("High Score: %d", enemy_destroyed_func.highScore);
      ImGui::Text("Wave: %d", enemy_destroyed_func.wave);
      ImGui::Text("Highest wave achieved: %d", enemy_destroyed_func.highestWave);
      ImGui::Text("Enemies remaining: %d", enemy_destroyed_func.enemiesLeft);*/
    
    }
    if (ImGui::Button("Main Menu"))
    {
      AudioPlayer::Play_Event(AK::EVENTS::PLAY_BUTTONBACK);
      Get<SceneSys>()->SetNextScene(SceneE::MenuScene);
    }
    if (ImGui::Button("Quit"))
    {
      AudioPlayer::Play_Event(AK::EVENTS::PLAY_BUTTONBACK);
      Get<SceneSys>()->Quit();
    }

    ImGui::End();
  }
  else
  {
    Get<FramerateController>()->Unpause();
    SDL_SetRelativeMouseMode(SDL_TRUE);
    {
      /* GameEntity& wind_tunnel_entity = entsys->GetEntity(wind_tunnel_id);
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

          if (othbeh.type == BHVRType::BEAR || othbeh.type == BHVRType::DUCK || othbeh.type == BHVRType::BUNNY)
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
    */
    }

    auto* input = Get<InputHandler>();
    auto& camera = Get<CameraSys>()->GetFPSCamera();

    auto& crossbow_trans = Get<ComponentSys>()->GetTransform(
      Get<EntitySys>()->GetEntity(crossbow_id).GetComponentHandle(ComponentKind::Transform));
//enable this to be able to move and rotate the weapon, and scale the weapon
#if 0
    dx::XMVECTOR newRot {};
    dx::XMVECTOR currRot = dx::XMLoadFloat4(&crossbow_trans.rotation);

    if (input->KeyPressedOrHeld(SDLK_KP_4))
    {
      newRot.m128_f32[0] += RAD(1.f);
    }
    if (input->KeyPressedOrHeld(SDLK_KP_5))
    {
      newRot.m128_f32[1] += RAD(1.f);
    }
    if (input->KeyPressedOrHeld(SDLK_KP_6))
    {
      newRot.m128_f32[2] += RAD(1.f);
    }
    if (input->KeyPressedOrHeld(SDLK_KP_7))
    {
      newRot.m128_f32[0] -= RAD(1.f);
    }
    if (input->KeyPressedOrHeld(SDLK_KP_8))
    {
      newRot.m128_f32[1] -= RAD(1.f);
    }
    if (input->KeyPressedOrHeld(SDLK_KP_9))
    {
      newRot.m128_f32[2] -= RAD(1.f);
    }
    currRot = dx::XMQuaternionMultiply(
      dx::XMQuaternionRotationRollPitchYaw(newRot.m128_f32[1], newRot.m128_f32[2], newRot.m128_f32[0]),
      currRot);
    dx::XMStoreFloat4(&crossbow_trans.rotation, currRot);
    if (input->KeyPressedOrHeld(SDLK_KP_1))
    {
      crossbow_trans.pos.x += 0.01f;
    }
    if (input->KeyPressedOrHeld(SDLK_KP_2))
    {
      crossbow_trans.pos.y += 0.01f;
    }
    if (input->KeyPressedOrHeld(SDLK_KP_3))
    {
      crossbow_trans.pos.z += 0.01f;
    }
    if (input->KeyPressedOrHeld(SDLK_KP_MINUS))
    {
      crossbow_trans.pos.x -= 0.01f;
    }
    if (input->KeyPressedOrHeld(SDLK_KP_PLUS))
    {
      crossbow_trans.pos.y -= 0.01f;
    }
    if (input->KeyPressedOrHeld(SDLK_KP_ENTER))
    {
      crossbow_trans.pos.z -= 0.01f;
    }
    if (input->KeyPressedOrHeld(SDLK_p))
    {
      crossbow_trans.scale.x += 0.01f;
      crossbow_trans.scale.y += 0.01f;
      crossbow_trans.scale.z += 0.01f;
    }
    if (input->KeyPressedOrHeld(SDLK_o))
    {
      crossbow_trans.scale.x -= 0.01f;
      crossbow_trans.scale.y -= 0.01f;
      crossbow_trans.scale.z -= 0.01f;
    }
#endif

//Input area
#if 1

    //shoot paper airplanes
    if (continue_shoot == false)
    {
      if (input->MouseButtonPressed(InputHandler::MouseButton::LEFT) && can_shoot)
      {
        Octane::AudioPlayer::Play_Event(AK::EVENTS::PLAY_CROSSBOW);
        can_shoot = false;
        auto& pos = compsys->GetTransform(entsys->GetPlayer()->GetComponentHandle(ComponentKind::Transform)).pos;
        float offsetFactor = 0.2f;
        auto offsetdir = camera.GetViewDirection();
        dx::XMFLOAT3 dir;
        dx::XMStoreFloat3(&dir, dx::XMVectorScale(offsetdir, offsetFactor));

        create_plane({pos.x + dir.x, pos.y + dir.y, pos.z + dir.z});
      }
    }
    else
    {
      if (input->MouseButtonPressedOrHeld(InputHandler::MouseButton::LEFT) && can_shoot)
      {
        Octane::AudioPlayer::Play_Event(AK::EVENTS::PLAY_CROSSBOW);
        can_shoot = false;
        auto& pos = compsys->GetTransform(entsys->GetPlayer()->GetComponentHandle(ComponentKind::Transform)).pos;
        float offsetFactor = 0.2f;
        auto offsetdir = camera.GetViewDirection();
        dx::XMFLOAT3 dir;
        dx::XMStoreFloat3(&dir, dx::XMVectorScale(offsetdir, offsetFactor));

        create_plane({pos.x + dir.x, pos.y + dir.y, pos.z + dir.z});
      }
    }

    if (input->KeyPressed(SDLK_f))
    {
      auto& weapon = entsys->GetEntity(crossbow_id);
      auto& weapon_render = compsys->GetRender(weapon.GetComponentHandle(ComponentKind::Render));

      if (weapon_render.mesh_type == Mesh_Key {"Sniper1"})
      {
        weapon_render.mesh_type = Mesh_Key {"Semiauto1"};

        /********************Unable below line to turn off auto mode*********************/
        continue_shoot = true;
        /********************Unable upper line to turn off auto mode*********************/
      }
      else if (weapon_render.mesh_type == Mesh_Key {"Semiauto1"})
      {
        continue_shoot = false;
        weapon_render.mesh_type = Mesh_Key {"Crossbow"};
        crossbow_trans.pos = {0.5f, -0.9f, .0f};
        crossbow_trans.scale = {1.f, 1.f, 1.f};
      }
      else if (weapon_render.mesh_type == Mesh_Key {"Crossbow"})
      {
        weapon_render.mesh_type = Mesh_Key {"Slingshot"};
        crossbow_trans.pos = {0.7f, -0.9f, .0f};
        crossbow_trans.scale = {0.6f, 0.6f, 0.6f};
        crossbow_trans.rotation = {-0.0605163462f, -0.753363013f, -0.113361359f, -0.644904613f};
      }
      else if (weapon_render.mesh_type == Mesh_Key {"Slingshot"})
      {
        weapon_render.mesh_type = Mesh_Key {"Sword"};
        crossbow_trans.pos = {0.5f, -0.9f, .0f};
        crossbow_trans.scale = {1.63f, 1.63f, 1.63f};
      }
      else if (weapon_render.mesh_type == Mesh_Key {"Sword"})
      {
        weapon_render.mesh_type = Mesh_Key {"Sniper1"};
      }
    }
#endif

    if (enemy_destroyed_func.spawnedWave)
    {
      AudioPlayer::Set_Position(spawner, {0.f, 1.f, 0.f});
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
  continue_shoot = false;

  SDL_SetRelativeMouseMode(SDL_FALSE);
  auto* entsys = Get<EntitySys>();
  auto* compsys = Get<ComponentSys>();

  entsys->FreeAllEntities();

  AudioPlayer::Unregister_Object(spawner);
}

SceneE TestScene::GetEnum() const
{
  return SceneE::TestScene;
}

} // namespace Octane
