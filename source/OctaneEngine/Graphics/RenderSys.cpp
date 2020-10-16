/******************************************************************************/
/*!
  \par        Project Octane
  \file       <THIS FILENAME>
  \author     <WHO WROTE THIS>
  \date       YYYY/MM/DD
  \brief      <WHAT DOES IT DO>

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#include <OctaneEngine/Graphics/RenderSys.h>

#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/Graphics/CameraSys.h>
#include <OctaneEngine/Graphics/OBJParser.h>
#include <OctaneEngine/SystemOrder.h>
#include <OctaneEngine/WindowManager.h>

namespace dx = DirectX;

namespace Octane
{
RenderSys::RenderSys(Engine* parent_engine)
  : ISystem(parent_engine),
    device_dx11_ {parent_engine->GetSystem<WindowManager>()->GetHandle()}
{
  Shader phong
    = device_dx11_.CreateShader(L"assets/shaders/phong.hlsl", Shader::InputLayout_POS | Shader::InputLayout_NOR);

  OBJParser obj_parser;
  auto addMesh = [=, &obj_parser](MeshType m, wchar_t const* filepath) {
    meshes_.insert(meshes_.begin() + to_integral(m), device_dx11_.CreateMesh(obj_parser.ParseOBJ(filepath)));
  };

  // TODO: asset loading system instead of this

  addMesh(MeshType::Cube, L"assets/models/cube.obj");
  addMesh(MeshType::Sphere, L"assets/models/sphere.obj");
  addMesh(MeshType::Cube_Rounded, L"assets/models/cube_rounded.obj");
  addMesh(MeshType::Bear, L"assets/models/Bear.obj");
  addMesh(MeshType::Duck, L"assets/models/Duck.obj");
  addMesh(MeshType::Crossbow, L"assets/models/PaperCrossbow.obj");
  addMesh(MeshType::Plane, L"assets/models/PaperPlane.obj");
  addMesh(MeshType::Shuriken, L"assets/models/PaperShuriken.obj");
  addMesh(MeshType::Stack, L"assets/models/PaperStack.obj");

  device_dx11_.GetD3D11Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void RenderSys::Update()
{
  DirectX::XMMATRIX cam_view_matrix = Get<CameraSys>()->GetViewMatrix();
  DirectX::XMMATRIX cam_projection_matrix = Get<CameraSys>()->GetProjectionMatrix();

  device_dx11_.ShaderConstants()
    .PerFrame()
    .SetViewProjection(dx::XMMatrixTranspose(cam_view_matrix * cam_projection_matrix))
    .SetCameraPosition(Get<CameraSys>()->GetFPSCamera().GetPosition());
  device_dx11_.Upload(device_dx11_.ShaderConstants().PerFrame());

  device_dx11_.ClearScreen();

  // Render all objects
  auto* component_sys = Get<ComponentSys>();
  MeshType current_mesh = MeshType::COUNT;
  for (GameEntity* iter = Get<EntitySys>()->EntitiesBegin(); iter != Get<EntitySys>()->EntitiesEnd(); ++iter)
  {
    if (iter->active)
    {
      auto& transform = component_sys->GetTransform(iter->GetComponentHandle(ComponentKind::Transform));
      auto& render_comp = component_sys->GetRender(iter->GetComponentHandle(ComponentKind::Render));

      dx::XMMATRIX object_world_matrix = dx::XMMatrixIdentity();
      float scale = transform.scale;
      object_world_matrix *= dx::XMMatrixScaling(scale, scale, scale);
      object_world_matrix *= dx::XMMatrixRotationAxis({0, 1, 0}, dx::XMConvertToRadians(transform.rotation));
      dx::XMFLOAT3 pos = transform.pos;
      object_world_matrix *= dx::XMMatrixTranslation(pos.x, pos.y, pos.z);

      if (current_mesh != render_comp.mesh_type)
      {
        current_mesh = render_comp.mesh_type;
        device_dx11_.UseMesh(meshes_[to_integral(current_mesh)]);
      }
      device_dx11_.ShaderConstants()
        .PerObject()
        .SetWorldMatrix(dx::XMMatrixTranspose(object_world_matrix))
        .SetWorldNormalMatrix(dx::XMMatrixInverse(nullptr, object_world_matrix))
        .SetColor(render_comp.color);
      device_dx11_.Upload(device_dx11_.ShaderConstants().PerObject());
      device_dx11_.DrawMesh();
    }
  }

  // Render ImGui


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
