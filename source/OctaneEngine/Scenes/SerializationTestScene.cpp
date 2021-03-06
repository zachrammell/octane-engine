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
#include <OctaneEngine/Physics/PhysicsSys.h>
#include <OctaneEngine/Graphics/TextureSys.h>
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

#include <OctaneEngine/Graphics/CameraSys.h>

#include <DirectXMath.h>

//#pragma comment(lib, "EASTL.lib")

namespace dx = DirectX;

namespace Octane
{
SerializationTestScene::SerializationTestScene(SceneSys* parent) : IScene(parent) {}

void SerializationTestScene::Load()
{
  auto* entsys = Get<EntitySys>();
  auto* compsys = Get<ComponentSys>();

  //auto create_object = [=](dx::XMFLOAT3 pos, dx::XMFLOAT3 scale, Color color, Mesh_Key mesh_type = Mesh_Key {"Cube"}) {
  //  // todo: custom entityid / componentid types with overridden operator*, because this is way too much boilerplate
  //  EntityID const ent_id = entsys->MakeEntity();
  //  GameEntity& ent = entsys->GetEntity((ent_id));
  //  ComponentHandle const trans_id = compsys->MakeTransform();
  //  ent.components[to_integral(ComponentKind::Transform)] = trans_id;
  //  TransformComponent& trans = compsys->GetTransform(trans_id);
  //  trans.pos = pos;
  //  trans.scale = scale;
  //  trans.rotation = {};
  //  ComponentHandle const render_id = compsys->MakeRender();
  //  ent.components[to_integral(ComponentKind::Render)] = render_id;
  //  RenderComponent& render_component = compsys->GetRender(render_id);
  //  render_component.material.diffuse = color;
  //  render_component.mesh_type = mesh_type;
  //  render_component.render_type = (rand() % 2 == 0) ? RenderType::Filled : RenderType::Wireframe;
  //  return ent_id;
  //};

  /*for (int i = 0; i < 100; ++i)
  {
    float scale = 0.25f * (i % 8);
    auto random_pos = []() {
      return 100.0f * (rand() / (float)RAND_MAX - 0.5f);
    };
    create_object(
      {random_pos(), random_pos(), random_pos()},
      {scale, scale, scale},
      Colors::db32[i % 32],
      magic_enum::enum_cast<MeshType>(rand() % to_integral(MeshType::COUNT)).value_or(MeshType::Cube));
  }*/
}

void SerializationTestScene::Start()
{
  Get<RenderSys>()->SetClearColor(Colors::db32[0]);
}

void SerializationTestScene::Update(float dt)
{
  auto& input = *Get<InputHandler>();

  if (camera_moving_)
  {
    if (input.KeyPressed(SDLK_ESCAPE))
    {
      camera_moving_ = false;
      SDL_SetRelativeMouseMode(SDL_FALSE);
    }
    dx::XMFLOAT3 cam_velocity;
    cam_velocity.x = 1.0f * (input.KeyHeld(SDLK_a) - input.KeyHeld(SDLK_d));
    cam_velocity.y = 1.0f * (input.KeyHeld(SDLK_SPACE) - input.KeyHeld(SDLK_LSHIFT));
    cam_velocity.z = 1.0f * (input.KeyHeld(SDLK_w) - input.KeyHeld(SDLK_s));

    dx::XMStoreFloat3(&cam_velocity, dx::XMVectorScale(dx::XMVector3Normalize(dx::XMLoadFloat3(&cam_velocity)), 0.25f));

    dx::XMINT2 mouse_vel = input.GetMouseMovement();
    auto& camera = Get<CameraSys>()->GetFPSCamera();
    camera.RotatePitchRelative(-mouse_vel.y * 1.0f);
    camera.RotateYawRelative(mouse_vel.x * 1.0f);

    camera.MoveRelativeToView(dx::XMLoadFloat3(&cam_velocity));
  }
  else if (input.MouseButtonReleased(InputHandler::MouseButton::LEFT))
  {
    camera_moving_ = true;
    SDL_SetRelativeMouseMode(SDL_TRUE);
  }

  auto* entsys = Get<EntitySys>();
  auto* compsys = Get<ComponentSys>();

  auto set_object = [=](
                      EntityID id,
                      dx::XMFLOAT3 pos,
                      dx::XMFLOAT3 scale,
                      dx::XMFLOAT3 rotation,
                      Color color,
                      RenderComponent& render_comp,
                      Mesh_Key mesh_type,
                      string_view name) {
    GameEntity& ent = entsys->GetEntity(id);
    ComponentHandle const trans_id = ent.GetComponentHandle(ComponentKind::Transform);
    TransformComponent& trans = compsys->GetTransform(trans_id);
    trans.pos = pos;
    trans.scale = scale;
    dx::XMStoreFloat4(&(trans.rotation), dx::XMQuaternionRotationRollPitchYawFromVector(dx::XMLoadFloat3(&rotation)));

    render_comp.material.diffuse = color;
    if (!name.empty())
    {
      if (!ent.HasComponent(ComponentKind::Metadata))
      {
        ent.components[to_integral(ComponentKind::Metadata)] = compsys->MakeMetadata();
      }
      ComponentHandle const metadata_id = ent.GetComponentHandle(ComponentKind::Metadata);
      MetadataComponent& metadata_component = compsys->GetMetadata(metadata_id);
      metadata_component.name = name;
    }
  };

  auto create_object = [=](
                         dx::XMFLOAT3 pos,
                         dx::XMFLOAT3 scale,
                         dx::XMFLOAT3 rotation,
                         Color color,
                         Mesh_Key mesh_type = Mesh_Key {"Cube"},
                         string_view name = "") {
    // todo: custom entityid / componentid types with overridden operator*, because this is way too much boilerplate
    EntityID const ent_id = entsys->MakeEntity();
    GameEntity& ent = entsys->GetEntity((ent_id));
    ComponentHandle const trans_id = compsys->MakeTransform();
    ent.components[to_integral(ComponentKind::Transform)] = trans_id;
    ComponentHandle const render_id = compsys->MakeRender();
    ent.components[to_integral(ComponentKind::Render)] = render_id;
    RenderComponent& render_comp = compsys->GetRender(render_id);
    if (!name.empty())
    {
      ComponentHandle const metadata_id = compsys->MakeMetadata();
      ent.components[to_integral(ComponentKind::Metadata)] = metadata_id;
    }
    set_object(ent_id, pos, scale, rotation, color, render_comp,mesh_type, name);
  };
  auto load_object = [=](
                       EntityID id,
                       dx::XMFLOAT3& pos,
                       dx::XMFLOAT3& scale,
                       dx::XMFLOAT3& rotation,
                       Color& color,
                       Mesh_Key& mesh_type,
                       RenderComponent** render_comp,
                       eastl::string& name) {
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
    RenderComponent& render_component = compsys->GetRender(render_id);
    color = render_component.material.diffuse;
    mesh_type = render_component.mesh_type;
    *render_comp = &render_component;
    if (ent.HasComponent(ComponentKind::Metadata))
    {
      ComponentHandle const metadata_id = ent.GetComponentHandle(ComponentKind::Metadata);
      MetadataComponent const& metadata_component = compsys->GetMetadata(metadata_id);
      name = metadata_component.name;
    }
  };

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
      if (ImGui::MenuItem("Editor Settings"))
      {
        editor_settings_ = !editor_settings_;
      }
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

  if (editor_settings_ && ImGui::Begin("Editor Settings", &editor_settings_, ImGuiWindowFlags_AlwaysAutoResize))
  {
    ImGui::DragFloat("Slider Sensitivity", &slider_sensitivity, 0.001f, 0.001f, 1.0f);
    ImGui::End();
  }

  if (entity_creator_ && ImGui::Begin("Entity Creator", &entity_creator_, ImGuiWindowFlags_AlwaysAutoResize))
  {
    char name_buf[32];
    memcpy(name_buf, entity_creator_data_.name.data(), 32);
    ImGui::InputText("Name", name_buf, 32);
    entity_creator_data_.name = name_buf;
    if (entity_creator_data_.name == " ")
    {
      entity_creator_data_.name.clear();
    }
    ImGui::DragFloat3("Position", &(entity_creator_data_.position.x), slider_sensitivity);
    ImGui::DragFloat3("Scale", &(entity_creator_data_.scale.x), slider_sensitivity);
    ImGui::DragFloat3("Rotation", &(entity_creator_data_.rotation.x), slider_sensitivity * dx::XM_2PI / 360.0f);
    ImGui::ColorEdit3("Color", &(entity_creator_data_.color.r));
    auto& meshNames = Get<MeshSys>()->MeshNames();
    if (ImGui::BeginCombo("Mesh", entity_creator_data_.mesh.data(), ImGuiComboFlags_None))
    {
      for (auto const& mesh : meshNames) //magic_enum::enum_entries<MeshType>())
      {
        if (ImGui::Selectable(mesh.data()))
        {
          entity_creator_data_.mesh = mesh.data();
        }
      }
      ImGui::EndCombo();
    }
    if (ImGui::Button("Copy Selected Entity"))
    {
      entity_creator_data_ = entity_editor_data_;
    }
    if (ImGui::Button("Create Entity"))
    {
      create_object(
        entity_creator_data_.position,
        entity_creator_data_.scale,
        entity_creator_data_.rotation,
        entity_creator_data_.color,
        entity_creator_data_.mesh,
        entity_creator_data_.name);
    }
    ImGui::End();
  }

  if (entity_editor_ && ImGui::Begin("Entity Editor", &entity_editor_, ImGuiWindowFlags_AlwaysAutoResize))
  {
    if (ImGui::BeginCombo("Entity", eastl::to_string(entity_editor_data_.id).c_str(), ImGuiComboFlags_None))
    {
      for (auto iter = Get<EntitySys>()->EntitiesBegin(); iter != Get<EntitySys>()->EntitiesEnd(); ++iter)
      {
        if (iter->active && iter->HasComponent(ComponentKind::Transform) && iter->HasComponent(ComponentKind::Render))
        {
          auto id = iter.ID();
          eastl::string display = eastl::to_string(id);
          if (iter->HasComponent(ComponentKind::Metadata))
          {
            eastl::string& name = compsys->GetMetadata(iter->GetComponentHandle(ComponentKind::Metadata)).name;
            if (!(name.empty()))
            {
              display = name;
            }
          }
          if (ImGui::Selectable((display + "##unique_id").c_str()))
          {
            entity_editor_data_.id = id;
            load_object(
              id,
              entity_editor_data_.position,
              entity_editor_data_.scale,
              entity_editor_data_.rotation,
              entity_editor_data_.color,
              entity_editor_data_.mesh,
              &entity_editor_data_.render_comp,
              entity_editor_data_.name);
          }
        }
      }
      ImGui::EndCombo();
    }
    char name_buf[32];
    memcpy(name_buf, entity_editor_data_.name.data(), 32);
    ImGui::InputText("Name", name_buf, 32);
    entity_editor_data_.name = name_buf;
    if (entity_editor_data_.name == " ")
    {
      entity_editor_data_.name.clear();
    }
    ImGui::DragFloat3("Position", &(entity_editor_data_.position.x), slider_sensitivity);
    ImGui::DragFloat3("Scale", &(entity_editor_data_.scale.x), slider_sensitivity);
    if (ImGui::Checkbox("Box Collider", &entity_editor_data_.has_collider))
    {
    }
    ImGui::DragFloat3("Rotation", &(entity_editor_data_.rotation.x), slider_sensitivity * dx::XM_2PI / 360.0f);
    if(entity_editor_data_.render_comp)
    {
      ImGui::ColorEdit3("Color", &(entity_editor_data_.color.vec.x));
    }
    if (ImGui::BeginCombo("DB32 Color", ""))
    {
      for (int i = 0; i < 32; ++i)
      {
        Color col = Colors::db32[i];
        ImVec2 p_min = ImGui::GetCursorScreenPos();
        ImVec2 p_max = ImVec2(p_min.x + ImGui::GetContentRegionAvailWidth(), p_min.y + ImGui::GetFrameHeight());
        ImGui::GetWindowDrawList()->AddRectFilled(
          p_min,
          p_max,
          ImGui::ColorConvertFloat4ToU32(ImVec4 {col.r, col.g, col.b, 1.0f}));
        if (ImGui::Selectable(eastl::to_string(i).c_str()))
        {
          entity_editor_data_.color = col;
        }
      }
      ImGui::EndCombo();
    }
    if (ImGui::BeginCombo("Mesh", entity_editor_data_.mesh.data(), ImGuiComboFlags_None))
    {
      auto& meshes = Get<MeshSys>()->MeshNames();
      for (auto const& mesh : meshes)
      {
        if (ImGui::Selectable(mesh.data()))
        {
          entity_editor_data_.mesh = mesh.data();
        }
      }
      ImGui::EndCombo();
    }
    if(entity_editor_data_.render_comp)
    {
      if (ImGui::BeginCombo("Texture", entity_editor_data_.render_comp->material.diffuse_texture.data(), ImGuiComboFlags_None))
      {
        auto& textures = Get<TextureSys>()->TextureNames();
        for (auto const& texture : textures)
        {
          if (ImGui::Selectable(texture.data()))
          {
            entity_editor_data_.render_comp->material.diffuse_texture = texture.data();
          }
        }
        ImGui::EndCombo();
      }
      ImGui::DragFloat3("Specular", &(entity_editor_data_.render_comp->material.specular.r), slider_sensitivity);
      ImGui::DragFloat("Shininess", &entity_editor_data_.render_comp->material.specular_exp, slider_sensitivity);
      ImGui::Checkbox("Tint", &entity_editor_data_.render_comp->material.tint);
    }


    if (entity_editor_data_.id != INVALID_ENTITY)
    {
      set_object(
        entity_editor_data_.id,
        entity_editor_data_.position,
        entity_editor_data_.scale,
        entity_editor_data_.rotation,
        entity_editor_data_.color,
        *entity_editor_data_.render_comp,
        entity_editor_data_.mesh,
        entity_editor_data_.name);
      if (entity_editor_data_.has_collider)
      {
        GameEntity& entity = entsys->GetEntity(entity_editor_data_.id);
        if (!entity.HasComponent(ComponentKind::Physics))
        {
          TransformComponent& transform_component
            = compsys->GetTransform(entity.GetComponentHandle(ComponentKind::Transform));
          auto const scale = transform_component.scale;
          dx::XMFLOAT3 const half_scale = {scale.x / 2.0f, scale.y / 2.0f, scale.z / 2.0f};
          entity.GetComponentHandle(ComponentKind::Physics)
            = compsys->MakePhysicsBox(transform_component, half_scale, 0.0f);
        }
      }
    }
    ImGui::End();
  }

  if (save)
  {
    Trace::Log(DEBUG, "Saving entities.\n");
    NBTWriter nbt_writer {"assets/maps/level1.nbt"};

    EntitySys& entity_sys = *Get<EntitySys>();
    ComponentSys& component_sys = *Get<ComponentSys>();
    if (nbt_writer.BeginList("Entities"))
    {
      // for every object, write it out
      for (auto entity = entity_sys.EntitiesBegin(); entity != entity_sys.EntitiesEnd(); ++entity)
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

            if (entity->HasComponent(ComponentKind::Metadata))
            {
              MetadataComponent& metadata_component
                = component_sys.GetMetadata(entity->GetComponentHandle(ComponentKind::Metadata));
              nbt_writer.Write("Name", string_view {metadata_component.name});
            }

            if (entity->HasComponent(ComponentKind::Physics))
            {
              PhysicsComponent& physics_component
                = component_sys.GetPhysics(entity->GetComponentHandle(ComponentKind::Physics));
              nbt_writer.Write(magic_enum::enum_name(ComponentKind::Physics), physics_component);
            }

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
    // prevents retained data from overwriting loaded entities
    entity_editor_data_ = {};

    Trace::Log(DEBUG, "Loading entities.\n");
    NBTReader nbt_reader {"assets/maps/level1.nbt"};

    EntitySys& entity_sys = *Get<EntitySys>();
    ComponentSys& component_sys = *Get<ComponentSys>();

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
                auto tex = nbt_reader.ReadString("Texture");
                auto spec = nbt_reader.Read<Color>("Specular");

                auto tint = nbt_reader.ReadByte("Tint");
                auto specular_exp = nbt_reader.ReadFloat("Specular Exponent");
                //auto specular = nbt_reader.MaybeRead<Color>

                RenderComponent& render_component = entity_sys.AddRenderComp(
                  ent_id,
                  nbt_reader.Read<Color>("Color"),
                  *std::find(meshnames.begin(), meshnames.end(), nbt_reader.Read<Mesh_Key>("Mesh")),tex);

                render_component.material.specular = spec;
                render_component.material.tint = tint;
                render_component.material.specular_exp = specular_exp;
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
                float mass = nbt_reader.ReadFloat("Mass");
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
