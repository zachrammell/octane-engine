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
#include <OctaneEngine/ImGuiSys.h>
#include <OctaneEngine/SystemOrder.h>
#include <OctaneEngine/WindowManager.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace dx = DirectX;

namespace
{

Octane::Shader phong;

}

namespace Octane
{
RenderSys::RenderSys(Engine* parent_engine)
  : ISystem(parent_engine),
    device_dx11_ {parent_engine->GetSystem<WindowManager>()->GetHandle()}
{
  phong = device_dx11_.CreateShader(L"assets/shaders/phong.hlsl", Shader::InputLayout_POS | Shader::InputLayout_NOR);
  device_dx11_.UseShader(phong);

  meshes_.resize(to_integral(MeshType::COUNT));


	
  //OBJParser obj_parser;
  auto addMesh = [=](MeshType m, char const* filepath) {
    device_dx11_.EmplaceMesh(meshes_.data() + to_integral(m), LoadMesh(filepath)/*obj_parser.ParseOBJ(filepath)*/);
  };

  // TODO: asset loading system instead of this

  addMesh(MeshType::Cube, "assets/models/cube.obj");
  addMesh(MeshType::Sphere, "assets/models/sphere.obj");
  addMesh(MeshType::Cube_Rounded, "assets/models/cube_rounded.obj");
  addMesh(MeshType::Bear, "assets/models/Bear.obj");
  addMesh(MeshType::Duck, "assets/models/Duck.obj");
  addMesh(MeshType::Crossbow, "assets/models/Crossbow.obj");
  addMesh(MeshType::Plane, "assets/models/PaperPlane.obj");
  addMesh(MeshType::Shuriken, "assets/models/Shuriken.obj");
  addMesh(MeshType::Stack, "assets/models/PaperStack.obj");
  addMesh(MeshType::TestFBX, "assets/models/testfbx.fbx");

  device_dx11_.GetD3D11Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void RenderSys::Update()
{
  DirectX::XMMATRIX cam_view_matrix = Get<CameraSys>()->GetViewMatrix();
  DirectX::XMMATRIX cam_projection_matrix = Get<CameraSys>()->GetProjectionMatrix();

  device_dx11_.ShaderConstants()
    .PerFrame()
    .SetViewProjection(dx::XMMatrixTranspose(cam_view_matrix * cam_projection_matrix))
    .SetCameraPosition(Get<CameraSys>()->GetFPSCamera().GetPosition())
    .SetLightPosition({100, 100, 50});
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
      dx::XMFLOAT3 scale = transform.scale;
      object_world_matrix *= dx::XMMatrixScaling(scale.x, scale.y, scale.z);
      object_world_matrix *= dx::XMMatrixRotationQuaternion(dx::XMLoadFloat4(&(transform.rotation)));
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


Mesh RenderSys::LoadMesh(const char* path)
{
  Assimp::Importer importer;
  const aiScene* scene =  importer.ReadFile(path, aiProcess_Triangulate);
  Mesh new_mesh;
  if (scene)
  {
    ProcessNode(scene, scene->mRootNode, new_mesh);
  }

  return new_mesh;
}

void RenderSys::ProcessNode(const aiScene* scene, aiNode* node, Mesh& mesh)
{
  for (int i = 0; i < node->mNumMeshes; ++i)
  {
    aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
    ProcessMesh(aimesh, mesh);
  }

  for (int i = 0; i < node->mNumChildren; ++i)
  {
    ProcessNode(scene, node->mChildren[i], mesh);
  }
}

void RenderSys::ProcessMesh(aiMesh* mesh,Mesh& new_mesh)
{
  size_t start = new_mesh.vertex_buffer.size(); //start of next mesh in aiScene

  for (int i = 0; i < mesh->mNumVertices; ++i)
  {
    auto& mVert = mesh->mVertices[i];
    dx::XMFLOAT3 norm{};
    if (mesh->HasNormals())
    {
      norm = {mesh->mNormals[i].x,mesh->mNormals[i].y,mesh->mNormals[i].z};
    }
    Mesh::Vertex vert {{mVert.x, mVert.y, mVert.z}, norm};
    // TODO: implement texture coordinates
#if 0
        if (mesh->mTextureCoords && mesh->mTextureCoords[0])
    {
          vert.uv.x = mesh->mTextureCoords[0][i].x;
          vert.uv.y = mesh->mTextureCoords[0][i].y;
    }
#endif
    new_mesh.vertex_buffer.push_back(vert);
  }
  for (int i = 0; i < mesh->mNumFaces; ++i)
  {
    auto& face = mesh->mFaces[i];

    for (int j = 0; j < face.mNumIndices; ++j)
    {
      new_mesh.index_buffer.push_back(face.mIndices[j]+start);
    }
  }
}

	
} // namespace Octane
