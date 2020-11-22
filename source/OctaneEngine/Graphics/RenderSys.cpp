/******************************************************************************/
/*!
  \par        Project Octane
  \file       <THIS FILENAME>
  \author     <WHO WROTE THIS>
  \date       YYYY/MM/DD
  \brief      <WHAT DOES IT DO>

  Copyright � 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

#include <OctaneEngine/Graphics/RenderSys.h>

#include <OctaneEngine/Graphics/MeshSys.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Components/RenderComponent.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/Graphics/CameraSys.h>
#include <OctaneEngine/Graphics/OBJParser.h>
#include <OctaneEngine/ImGuiSys.h>
#include <OctaneEngine/SystemOrder.h>
#include <OctaneEngine/WindowManager.h>

#include <magic_enum.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

//#pragma comment(lib, "EASTL.lib")
namespace dx = DirectX;

namespace
{

Octane::Shader phong;
Octane::Shader ui;
}

namespace Octane
{
RenderSys::RenderSys(Engine* parent_engine)
  : ISystem(parent_engine),
    device_dx11_ {parent_engine->GetSystem<WindowManager>()->GetHandle()}
{
  phong = device_dx11_.CreateShader(L"assets/shaders/phong.hlsl", Shader::InputLayout_POS | Shader::InputLayout_NOR);
  ui = device_dx11_.CreateShader(L"assets/shaders/UI.hlsl", Shader::InputLayout_POS);
  device_dx11_.UseShader(phong);
  device_dx11_.GetD3D11Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void RenderSys::Update()
{
  DirectX::XMMATRIX cam_view_matrix = Get<CameraSys>()->GetViewMatrix();
  DirectX::XMMATRIX cam_projection_matrix = Get<CameraSys>()->GetProjectionMatrix();

  device_dx11_.ShaderConstants()
    .PerFrame()
    .SetViewProjection(cam_view_matrix * cam_projection_matrix)
    .SetCameraPosition(Get<CameraSys>()->GetFPSCamera().GetPosition())
    .SetLightPosition({100, 100, 50});
  device_dx11_.Upload(device_dx11_.ShaderConstants().PerFrame());

  device_dx11_.ClearScreen();

  // Render all objects
  auto* component_sys = Get<ComponentSys>();
  Mesh_Key current_mesh;
  auto* meshSys = reinterpret_cast<MeshSys*>(engine_.GetSystem(SystemOrder::MeshSys));
  auto& meshes_ = meshSys->Meshes();
  for (auto shader_type : magic_enum::enum_values<ShaderType>())
  {
    Shader* shader = nullptr;
    switch (shader_type)
    {
    case Octane::ShaderType::Phong:
      shader = &phong; break;
    case Octane::ShaderType::UI:
      shader = &ui; break;
    case Octane::ShaderType::COUNT:
      continue;
    default:
      continue;
    }
    device_dx11_.UseShader(*shader);
    for (auto render_type : magic_enum::enum_values<RenderType>())
    {
      switch (render_type)
      {
      case RenderType::Filled: device_dx11_.SetWireframeMode(false); break;
      case RenderType::Wireframe: device_dx11_.SetWireframeMode(true); break;
      default: break;
      }

      for (auto iter = Get<EntitySys>()->EntitiesBegin(); iter != Get<EntitySys>()->EntitiesEnd(); ++iter)
      {
        if (iter->active && iter->HasComponent(ComponentKind::Transform) && iter->HasComponent(ComponentKind::Render))
        {
          auto& transform = component_sys->GetTransform(iter->GetComponentHandle(ComponentKind::Transform));
          auto& render_comp = component_sys->GetRender(iter->GetComponentHandle(ComponentKind::Render));

          if (render_comp.render_type != render_type || render_comp.shader_type != shader_type)
          {
            continue;
          }

          dx::XMMATRIX object_world_matrix = dx::XMMatrixIdentity();
          dx::XMFLOAT3 scale = transform.scale;
          object_world_matrix *= dx::XMMatrixScaling(scale.x, scale.y, scale.z);
          object_world_matrix
            = object_world_matrix * dx::XMMatrixRotationQuaternion(dx::XMLoadFloat4(&(transform.rotation)));
          dx::XMFLOAT3 pos = transform.pos;
          object_world_matrix = object_world_matrix * dx::XMMatrixTranslation(pos.x, pos.y, pos.z);

          if (current_mesh != render_comp.mesh_type)
          {
            current_mesh = render_comp.mesh_type;
            auto mesh = meshes_.find(current_mesh.data());
            
            device_dx11_.UseMesh(*mesh->second);
          }
          device_dx11_.ShaderConstants()
            .PerObject()
            .SetWorldMatrix(object_world_matrix)
            .SetWorldNormalMatrix(dx::XMMatrixTranspose(dx::XMMatrixInverse(nullptr, object_world_matrix)))
            .SetColor(render_comp.color);
          device_dx11_.Upload(device_dx11_.ShaderConstants().PerObject());
          device_dx11_.DrawMesh();
        }
      }
    }
  }

  // Render ImGui
  Get<ImGuiSys>()->Render();

  // Present
  device_dx11_.Present();
}

SystemOrder RenderSys::GetOrder()
{
  return SystemOrder::RenderSys;
}

void RenderSys::SetClearColor(Color clear_color)
{
  device_dx11_.SetClearColor(clear_color);
}



} // namespace Octane
