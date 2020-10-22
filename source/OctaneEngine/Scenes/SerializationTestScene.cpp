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
#include <OctaneEngine/NBTSpecializations.h>
#include <OctaneEngine/NBTWriter.h>

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

  for (int i = 0; i < 2048; ++i)
  {
    float scale = 0.25 * (i % 8);
    auto random_pos = []() {
      return 100.0f * (rand() / (float)RAND_MAX - 0.5f);
    };
    create_object(
      {random_pos(), random_pos(), random_pos()},
      {scale, scale, scale},
      Colors::db32[i % 32],
      magic_enum::enum_cast<MeshType>(rand() % to_integral(MeshType::COUNT)).value_or(MeshType::Cube));
  }
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
      save = false;
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
            if (nbt_writer.BeginCompound(""))
            {
              nbt_writer.Write("Component", to_integral(ComponentKind::Render));
              nbt_writer.Write("Mesh", to_integral(render_component.mesh_type));
              nbt_writer.Write("Color", render_component.color);
              nbt_writer.EndCompound();
            }

            TransformComponent& transform_component
              = component_sys.GetTransform(entity->GetComponentHandle(ComponentKind::Transform));
            if (nbt_writer.BeginCompound(""))
            {
              nbt_writer.Write("Component", to_integral(ComponentKind::Transform));
              nbt_writer.Write("Pos", transform_component.pos);
              nbt_writer.Write("Scale", transform_component.scale);
              nbt_writer.Write("Rotation", transform_component.rotation);
              nbt_writer.EndCompound();
            }

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
    // for every object, read it in
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
