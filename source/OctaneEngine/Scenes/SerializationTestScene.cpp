/******************************************************************************/
/*!
  \par        Project Octane
  \file       SerializationTestScene.cpp
  \author     Zach Rammell
  \date       2020/10/21
  \brief      scene to test serialization

  Copyright � 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

// Main include

#include <OctaneEngine/Scenes/SerializationTestScene.h>

#include <OctaneEngine/Engine.h>

#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/Graphics/RenderSys.h>
#include <OctaneEngine/InputHandler.h>
#include <OctaneEngine/Trace.h>

#include <OctaneEngine/NBTReader.h>
#include <OctaneEngine/NBTSpecializations.h>
#include <OctaneEngine/NBTWriter.h>

#include <EASTL/optional.h>
#include <EASTL/string.h>
#include <EASTL/string_view.h>
#include <imgui.h>
#define MAGIC_ENUM_USING_ALIAS_OPTIONAL \
  template<typename T>                  \
  using optional = eastl::optional<T>;
#define MAGIC_ENUM_USING_ALIAS_STRING      using string = eastl::string;
#define MAGIC_ENUM_USING_ALIAS_STRING_VIEW using string_view = eastl::string_view;
#include <magic_enum.hpp>

#include <DirectXMath.h>

namespace dx = DirectX;

namespace Octane
{
SerializationTestScene::SerializationTestScene(SceneSys* parent) : IScene(parent) {}

void SerializationTestScene::Load()
{
  auto* entsys = Get<EntitySys>();
  auto* compsys = Get<ComponentSys>();

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

  //for (int i = 0; i < 2048; ++i)
  //{
  //  float scale = 0.25f * (i % 8);
  //  auto random_pos = []() {
  //    return 100.0f * (rand() / (float)RAND_MAX - 0.5f);
  //  };
  //  create_object(
  //    {random_pos(), random_pos(), random_pos()},
  //    {scale, scale, scale},
  //    Colors::db32[i % 32],
  //    magic_enum::enum_cast<MeshType>(rand() % to_integral(MeshType::COUNT)).value_or(MeshType::Cube));
  //}
}

void SerializationTestScene::Start()
{
  Get<RenderSys>()->SetClearColor(Colors::db32[0]);
}

void SerializationTestScene::Update(float dt)
{
  auto* entsys = Get<EntitySys>();
  auto* compsys = Get<ComponentSys>();

  auto set_object =
    [=](EntityID id, dx::XMFLOAT3 pos, dx::XMFLOAT3 scale, dx::XMFLOAT3 rotation, Color color, MeshType mesh_type) {
      GameEntity& ent = entsys->GetEntity(id);
      ComponentHandle const trans_id = ent.GetComponentHandle(ComponentKind::Transform);
      TransformComponent& trans = compsys->GetTransform(trans_id);
      trans.pos = pos;
      trans.scale = scale;
      dx::XMStoreFloat4(&(trans.rotation), dx::XMQuaternionRotationRollPitchYawFromVector(dx::XMLoadFloat3(&rotation)));

      ComponentHandle const render_id = ent.GetComponentHandle(ComponentKind::Render);
      RenderComponent& render_component = compsys->GetRender(render_id);
      render_component.color = color;
      render_component.mesh_type = mesh_type;
    };

  auto create_object =
    [=](dx::XMFLOAT3 pos, dx::XMFLOAT3 scale, dx::XMFLOAT3 rotation, Color color, MeshType mesh_type = MeshType::Cube) {
      // todo: custom entityid / componentid types with overridden operator*, because this is way too much boilerplate
      EntityID const ent_id = entsys->MakeEntity();
      GameEntity& ent = entsys->GetEntity((ent_id));
      ComponentHandle const trans_id = compsys->MakeTransform();
      ent.components[to_integral(ComponentKind::Transform)] = trans_id;
      ComponentHandle const render_id = compsys->MakeRender();
      ent.components[to_integral(ComponentKind::Render)] = render_id;

      set_object(ent_id, pos, scale, rotation, color, mesh_type);
    };
  auto load_object = [=](
                       EntityID id,
                       dx::XMFLOAT3& pos,
                       dx::XMFLOAT3& scale,
                       dx::XMFLOAT3& rotation,
                       Color& color,
                       MeshType& mesh_type) {
    GameEntity& ent = entsys->GetEntity(id);
    ComponentHandle const trans_id = ent.GetComponentHandle(ComponentKind::Transform);
    TransformComponent const& trans = compsys->GetTransform(trans_id);
    pos = trans.pos;
    scale = trans.scale;

    dx::XMFLOAT4 q = trans.rotation;

    float yaw = atan2(2.0f * (q.y * q.z + q.w * q.x), q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z);
    float pitch = asin(-2.0f * (q.x * q.z - q.w * q.y));
    float roll = atan2(2.0f * (q.x * q.y + q.w * q.z), q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z);

    rotation = {yaw, pitch, roll};

    ComponentHandle const render_id = ent.GetComponentHandle(ComponentKind::Render);
    RenderComponent const& render_component = compsys->GetRender(render_id);
    color = render_component.color;
    mesh_type = render_component.mesh_type;
  };

  InputHandler& input = *Get<InputHandler>();
  bool save = false, load = false;
  if (input.KeyHeld(SDLK_LCTRL) || input.KeyHeld(SDLK_RCTRL))
  {
    // Ctrl-S: Save
    if (input.KeyPressed(SDLK_s))
    {
      save = true;
    }
    // Ctrl-L: Load
    if (input.KeyPressed(SDLK_l))
    {
      load = true;
    }
  }

  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      if (ImGui::MenuItem("Save"))
      {
        save = true;
      }
      if (ImGui::MenuItem("Load"))
      {
        load = true;
      }
      if (ImGui::MenuItem("Quit"))
      {
        parent_manager_.Quit();
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View"))
    {
      if (ImGui::MenuItem("Entity Creator"))
      {
        entity_creator_ = !entity_creator_;
      }
      if (ImGui::MenuItem("Entity Editor"))
      {
        entity_editor_ = !entity_editor_;
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  if (entity_creator_ && ImGui::Begin("Entity Creator", &entity_creator_, ImGuiWindowFlags_AlwaysAutoResize))
  {
    ImGui::DragFloat3("Position", &(entity_creator_data_.position.x), 0.25f);
    ImGui::DragFloat3("Scale", &(entity_creator_data_.scale.x), 0.25f);
    ImGui::DragFloat3("Rotation", &(entity_creator_data_.rotation.x), dx::XM_2PI / 360.0f);
    ImGui::ColorEdit3("Color", &(entity_creator_data_.color.r));
    if (ImGui::BeginCombo("Mesh", magic_enum::enum_name(entity_creator_data_.mesh).data(), ImGuiComboFlags_None))
    {
      for (auto const& mesh : magic_enum::enum_entries<MeshType>())
      {
        if (mesh.first == MeshType::COUNT || mesh.first == MeshType::INVALID)
        {
          continue;
        }
        if (ImGui::Selectable(mesh.second.data()))
        {
          entity_creator_data_.mesh = mesh.first;
        }
      }
      ImGui::EndCombo();
    }
    if (ImGui::Button("Create Entity"))
    {
      create_object(
        entity_creator_data_.position,
        entity_creator_data_.scale,
        entity_creator_data_.rotation,
        entity_creator_data_.color,
        entity_creator_data_.mesh);
    }
    ImGui::End();
  }

  if (entity_editor_ && ImGui::Begin("Entity Editor", &entity_editor_, ImGuiWindowFlags_AlwaysAutoResize))
  {
    if (ImGui::BeginCombo("ID", eastl::to_string(entity_editor_data_.id).c_str(), ImGuiComboFlags_None))
    {
      for (GameEntity* iter = Get<EntitySys>()->EntitiesBegin(); iter != Get<EntitySys>()->EntitiesEnd(); ++iter)
      {
        if (iter->active && iter->HasComponent(ComponentKind::Transform) && iter->HasComponent(ComponentKind::Render))
        {
          auto id = iter - Get<EntitySys>()->EntitiesBegin();
          if (ImGui::Selectable(eastl::to_string(id).c_str()))
          {
            entity_editor_data_.id = id;
            load_object(
              id,
              entity_editor_data_.position,
              entity_editor_data_.scale,
              entity_editor_data_.rotation,
              entity_editor_data_.color,
              entity_editor_data_.mesh);
          }
        }
      }
      ImGui::EndCombo();
    }

    ImGui::DragFloat3("Position", &(entity_editor_data_.position.x), 0.25f);
    ImGui::DragFloat3("Scale", &(entity_editor_data_.scale.x), 0.25f);
    ImGui::DragFloat3("Rotation", &(entity_editor_data_.rotation.x), dx::XM_2PI / 360.0f);
    ImGui::ColorEdit3("Color", &(entity_editor_data_.color.r));
    if (ImGui::BeginCombo("Mesh", magic_enum::enum_name(entity_editor_data_.mesh).data(), ImGuiComboFlags_None))
    {
      for (auto const& mesh : magic_enum::enum_entries<MeshType>())
      {
        if (mesh.first == MeshType::COUNT || mesh.first == MeshType::INVALID)
        {
          continue;
        }
        if (ImGui::Selectable(mesh.second.data()))
        {
          entity_creator_data_.mesh = mesh.first;
        }
      }
      ImGui::EndCombo();
    }

    if (entity_editor_data_.id != -1u)
    {
      set_object(
        entity_editor_data_.id,
        entity_editor_data_.position,
        entity_editor_data_.scale,
        entity_editor_data_.rotation,
        entity_editor_data_.color,
        entity_editor_data_.mesh);
    }
    ImGui::End();
  }

  if (save)
  {
    Trace::Log(DEBUG, "Saving entities.\n");
    NBTWriter nbt_writer {"sandbox/serialization_test.nbt"};

    EntitySys& entity_sys = *Get<EntitySys>();
    ComponentSys& component_sys = *Get<ComponentSys>();
    if (nbt_writer.BeginList("Entities"))
    {
      // for every object, write it out
      for (GameEntity* entity = entity_sys.EntitiesBegin(); entity != entity_sys.EntitiesEnd(); ++entity)
      {
        if (entity->active)
        {
          if (nbt_writer.BeginCompound(""))
          {
            RenderComponent& render_component
              = component_sys.GetRender(entity->GetComponentHandle(ComponentKind::Render));
            nbt_writer.Write(magic_enum::enum_name(ComponentKind::Render), render_component);

            TransformComponent& transform_component
              = component_sys.GetTransform(entity->GetComponentHandle(ComponentKind::Transform));
            nbt_writer.Write(magic_enum::enum_name(ComponentKind::Transform), transform_component);

            // automatically get all components (needs generic component interface)
            /*for (auto component_type : magic_enum::enum_values<ComponentKind>())
            {
              auto component_handle = entity->GetComponentHandle(component_type);
              if (component_handle != -1u)
              {
                Component& component = component_sys.GetComponent(component_type, component_handle);
                nbt_writer.Write(magic_enum::enum_name(component_type), component);
              }
            }*/
            nbt_writer.EndCompound();
          }
        }
      }
      nbt_writer.EndList();
    }
  }
  if (load)
  {
    Get<EntitySys>()->FreeAllEntities();

    Trace::Log(DEBUG, "Loading entities.\n");
    EntitySys& entity_sys = *Get<EntitySys>();
    ComponentSys& component_sys = *Get<ComponentSys>();

    Trace::Log(DEBUG, "Loading entities.\n");
    NBTReader nbt_reader {"sandbox/serialization_test.nbt"};

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
  }
}

void SerializationTestScene::End() {}

void SerializationTestScene::Unload()
{
  Get<EntitySys>()->FreeAllEntities();
}

SceneE SerializationTestScene::GetEnum() const
{
  return SceneE::MenuScene;
}

} // namespace Octane
