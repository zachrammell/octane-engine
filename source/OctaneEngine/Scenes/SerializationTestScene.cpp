/******************************************************************************/
/*!
  \par        Project Octane
  \file       SerializationTestScene.cpp
  \author     Zach Rammell
  \date       2020/10/21
  \brief      scene to test serialization

  Copyright © 2020 DigiPen, All rights reserved.
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
#include <OctaneEngine/NBTWriter.h>
#include <OctaneEngine/NBTSpecializations.h>

#include <EASTL/string.h>
#include <imgui.h>
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
    ImGui::EndMainMenuBar();
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
          if (nbt_writer.BeginList(""))
          {
            RenderComponent& render_component
              = component_sys.GetRender(entity->GetComponentHandle(ComponentKind::Render));
            nbt_writer.Write("", render_component);

            TransformComponent& transform_component
              = component_sys.GetTransform(entity->GetComponentHandle(ComponentKind::Transform));
            nbt_writer.Write("", transform_component);

            /* automatically get all components (needs generic component interface)
         *for (auto component : magic_enum::enum_values<ComponentKind>())
        {
          if ()
          {
          }
        }*/
            nbt_writer.EndList();
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
    auto create_object = [=, &entity_sys, &component_sys](dx::XMFLOAT3 pos, dx::XMFLOAT3 scale, Color color, MeshType mesh_type = MeshType::Cube) {
      // todo: custom entityid / componentid types with overridden operator*, because this is way too much boilerplate
      EntityID const ent_id = entity_sys.MakeEntity();
      GameEntity& ent = entity_sys.GetEntity((ent_id));
      ComponentHandle const trans_id = component_sys.MakeTransform();
      ent.components[to_integral(ComponentKind::Transform)] = trans_id;
      TransformComponent& trans = component_sys.GetTransform(trans_id);
      trans.pos = pos;
      trans.scale = scale;
      trans.rotation = {};
      ComponentHandle const render_id = component_sys.MakeRender();
      ent.components[to_integral(ComponentKind::Render)] = render_id;
      RenderComponent& render_component = component_sys.GetRender(render_id);
      render_component.color = color;
      render_component.mesh_type = mesh_type;
    };

    Trace::Log(DEBUG, "Loading entities.\n");
    NBTReader nbt_reader {"sandbox/serialization_test.nbt"};

    // for every object, read it in
    if (nbt_reader.EnterList("Entities"))
    {
      const int entity_list_size = nbt_reader.ListSize();
      for (int i = 0; i < entity_list_size; ++i)
      {
        if (nbt_reader.EnterList(""))
        {
          EntityID const ent_id = entity_sys.MakeEntity();
          GameEntity& ent = entity_sys.GetEntity((ent_id));
          if (nbt_reader.EnterCompound(""))
          {
            ComponentHandle const render_id = component_sys.MakeRender();
            ent.components[to_integral(ComponentKind::Render)] = render_id;
            RenderComponent& render_component = component_sys.GetRender(render_id);
            render_component.color = nbt_reader.Read<Color>("Color");
            render_component.mesh_type = nbt_reader.Read<MeshType>("Mesh");
            nbt_reader.ExitList();
          }
          if (nbt_reader.EnterCompound(""))
          {
            ComponentHandle const trans_id = component_sys.MakeTransform();
            ent.components[to_integral(ComponentKind::Transform)] = trans_id;
            TransformComponent& trans = component_sys.GetTransform(trans_id);
            trans.pos = nbt_reader.Read<DirectX::XMFLOAT3>("Pos");
            trans.scale = nbt_reader.Read<DirectX::XMFLOAT3>("Scale");
            trans.rotation = nbt_reader.Read<DirectX::XMFLOAT4>("Rotation");
            nbt_reader.ExitList();
          }
          nbt_reader.ExitList();
        }
      }
      nbt_reader.ExitList();
    }
    __debugbreak();
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
