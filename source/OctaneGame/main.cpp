#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>

#include <iostream>

#define SDL_MAIN_HANDLED
#include <EASTL/vector.h>
#include <SDL.h>
#include <SDL_syswm.h>

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_sdl.h>

#include <OctaneEngine/Engine.h>
#include <OctaneEngine/Graphics/OBJParser.h>
#include <OctaneEngine/Graphics/RenderDX11.h>
#include <OctaneEngine/Style.h>

#include <OctaneEngine/FramerateController.h>
#include <OctaneEngine/InputHandler.h>
#include <OctaneEngine/Physics/World.h>
#include <OctaneEngine/WindowManager.h>

// EASTL expects user-defined new[] operators that it will use for memory allocation.
// TODO: make these return already-allocated memory from our own memory allocator.
void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
  return new uint8_t[size];
}

void* operator new[](
  size_t size,
  size_t alignment,
  size_t alignmentOffset,
  const char* pName,
  int flags,
  unsigned debugFlags,
  const char* file,
  int line)
{
  return new uint8_t[size];
}

namespace
{
winrt::com_ptr<ID3D11Buffer> constant_buffers[2];

struct ScaleRotation
{
  float scale;
  float rotation;
};

constexpr int MAX_OBJECTS = 512;
DirectX::XMFLOAT3 object_positions[MAX_OBJECTS] {};
ScaleRotation object_scale_rotations[MAX_OBJECTS] {};
Octane::Color object_colors[MAX_OBJECTS] {};
int object_active[MAX_OBJECTS] {};
} // namespace

int main(int argc, char* argv[]) noexcept
{
  // must be the first thing in main, for SDL2 initialization
  SDL_SetMainReady();

  std::clog << "[== Project Octane ==]\n";
  Octane::Engine engine;
  engine.AddSystem(new Octane::FramerateController {&engine});
  engine.AddSystem(new Octane::InputHandler {&engine});
  engine.AddSystem(new Octane::WindowManager {&engine, "Project Octane", 1280, 720});
  engine.AddSystem(new Octane::World {&engine});

  std::clog << "Initializing DX11\n";
  Octane::RenderDX11 render {engine.GetSystem<Octane::WindowManager>()->GetHandle()};
  Octane::Shader phong = render.CreateShader(
    L"assets/shaders/phong.hlsl",
    Octane::Shader::InputLayout_POS | Octane::Shader::InputLayout_NOR);
  render.SetClearColor(Octane::Colors::cerulean);

  Octane::OBJParser obj_parser;
  Octane::Mesh m = obj_parser.ParseOBJ(L"assets/models/cube_rounded.obj");
  Octane::MeshDX11 m_dx11 = render.CreateMesh(m);

  struct cb_per_object
  {
    DirectX::XMFLOAT4X4 World;
    DirectX::XMFLOAT4X4 NormalWorld;
    DirectX::XMFLOAT3 Color;
    FLOAT padding0_;
  } cb_per_obj;

  {
    D3D11_BUFFER_DESC
    cb_buffer_descriptor {sizeof(cb_per_object), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0, 0};
    HRESULT hr = render.GetD3D11Device()->CreateBuffer(&cb_buffer_descriptor, nullptr, constant_buffers[0].put());
    assert(SUCCEEDED(hr));
  }

  struct cb_per_frame
  {
    DirectX::XMFLOAT4X4 ViewProjection;
    DirectX::XMFLOAT3 CameraPosition;
    FLOAT padding0_;
    DirectX::XMFLOAT3 LightPosition;
    FLOAT padding1_;
  } cb_per_fr;

  {
    D3D11_BUFFER_DESC
    cb_buffer_descriptor {sizeof(cb_per_frame), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0, 0};
    HRESULT hr = render.GetD3D11Device()->CreateBuffer(&cb_buffer_descriptor, nullptr, constant_buffers[1].put());
    if (FAILED(hr))
    {
      std::cout << std::system_category().message(hr);
    }
  }

  render.GetD3D11Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  render.UseMesh(m_dx11);

  DirectX::XMMATRIX cam_view_matrix;
  DirectX::XMMATRIX cam_projection_matrix;

  DirectX::XMFLOAT3 cam_position = {5.0f, 1.0f, 5.0f};
  DirectX::XMFLOAT3 cam_target = {0.0f, 0.0f, 0.0f};
  DirectX::XMVECTOR cam_up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

  DirectX::XMFLOAT3 light_position = {100.0f, 100.0f, 50.0f};

  for (int i = 0; i < 100; ++i)
  {
    object_active[i] = true;
    object_colors[i] = Octane::Colors::peach;
    object_positions[i].x = 0.25f * (i - 50);
    object_positions[i].y = 0.01f * (i - 50) * (i - 50);
    object_positions[i].z = 0.33f * (i - 50);
    object_scale_rotations[i].scale = 0.25f;
  }

  auto world = engine.GetSystem<Octane::World>();

  object_active[100] = true;
  object_colors[100] = Octane::Colors::red;
  object_scale_rotations[100].scale = 0.25f;

  auto body = world->AddRigidBody();
  DirectX::XMFLOAT3 constraints = {1.0f, 1.0f, 1.0f};
  body->SetLinearConstraints(constraints);
  body->SetAngularConstraints(constraints);

  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows

  io.ConfigViewportsNoTaskBarIcon = true;

  //ImGui::StyleColorsDark();
  Octane::Style::ImguiVisualStudio();
  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  ImGui_ImplSDL2_InitForD3D(engine.GetSystem<Octane::WindowManager>()->GetHandle());
  ImGui_ImplDX11_Init(render.GetD3D11Device(), render.GetD3D11Context());

  bool scene_settings_open = false;
  bool demo_window_open = false;
  bool main_menu = true;
  bool esc_menu = false;

  while (!engine.ShouldQuit())
  {
    engine.Update();

    // This is extremely clean
    auto* frc = engine.GetSystem<Octane::FramerateController>();
    float dt = frc->GetDeltaTime();

    auto* input = engine.GetSystem<Octane::InputHandler>();

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplSDL2_NewFrame(engine.GetSystem<Octane::WindowManager>()->GetHandle());
    ImGui::NewFrame();
    if (main_menu)
    {
      ImGui::Begin(
        "menu",
        NULL,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
      ImGui::SetWindowPos("menu", ImVec2(800.0f, 500.0f));
      if (esc_menu == false)
      {
        if (ImGui::Button("play"))
        {
          main_menu = false;
        }
        render.SetClearColor(Octane::Colors::black);
        render.DrawScene();
      }
      else
      {
        if (ImGui::Button("resume"))
        {
          main_menu = false;
          esc_menu = false;
        }
        if (ImGui::Button("main menu"))
        {
          main_menu = true;
          esc_menu = false;
        }
      }
      if (ImGui::Button("quit"))
      {
        break;
      }
      //If press the esc, shut down the program
      if (input->KeyReleased(SDLK_ESCAPE))
      {
        break;
      }

      ImGui::End();
    }
    else
    {
      //If press the esc, make menu come out
      if (input->KeyReleased(SDLK_ESCAPE))
      {
        main_menu = true;
        esc_menu = true;
      }

      if (ImGui::BeginMainMenuBar())
      {
        if (ImGui::BeginMenu("File"))
        {
          if (ImGui::MenuItem("Quit"))
          {
            engine.Quit();
          }

          ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
          if (ImGui::BeginMenu("Theme"))
          {
            if (ImGui::MenuItem("Visual Studio"))
            {
              Octane::Style::ImguiVisualStudio();
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
            ImGui::MenuItem("Scene Settings", 0, &scene_settings_open);
            ImGui::MenuItem("ImGui Demo", 0, &demo_window_open);
            ImGui::EndMenu();
          }
          ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
      }

      if (scene_settings_open)
      {
        ImGui::Begin("Scene Settings", &scene_settings_open, ImGuiWindowFlags_AlwaysAutoResize);
        if (ImGui::CollapsingHeader("Light Properties"))
        {
          ImGui::DragFloat3("Light Position", (&light_position.x), 0.01f);
        }
        if (ImGui::CollapsingHeader("Camera Properties"))
        {
          ImGui::DragFloat3("Camera Position", (&cam_position.x), 0.01f);
          ImGui::DragFloat3("Camera Target", (&cam_target.x), 0.01f);
        }
        ImGui::End();
      }

      if (demo_window_open)
      {
        ImGui::ShowDemoWindow(&demo_window_open);
      }

      DirectX::XMFLOAT3 cam_velocity;
      cam_velocity.x = (input->KeyHeld(SDLK_LEFT) - input->KeyHeld(SDLK_RIGHT));
      cam_velocity.y = (input->KeyHeld(SDLK_SPACE) - input->KeyHeld(SDLK_LSHIFT));
      cam_velocity.z = (input->KeyHeld(SDLK_UP) - input->KeyHeld(SDLK_DOWN));

      //temp physics code

      DirectX::XMFLOAT3 sample_force;
      sample_force.x = 1.0f * (input->KeyHeld(SDLK_d) - input->KeyHeld(SDLK_a));
      sample_force.y = 1.0f * (input->KeyHeld(SDLK_w) - input->KeyHeld(SDLK_s));
      sample_force.z = 1.0f * (input->KeyHeld(SDLK_q) - input->KeyHeld(SDLK_e));

      body->ApplyForceCentroid(sample_force);
      body->SyncToPosition(object_positions[100]);

      DirectX::XMStoreFloat3(
        &cam_velocity,
        DirectX::XMVectorScale(DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&cam_velocity)), 0.5f));

      cam_position.x += cam_velocity.x;
      cam_position.y += cam_velocity.y;
      cam_position.z += cam_velocity.z;

      cam_view_matrix
        = DirectX::XMMatrixLookAtRH(DirectX::XMLoadFloat3(&cam_position), DirectX::XMLoadFloat3(&cam_target), cam_up);
      cam_projection_matrix = DirectX::XMMatrixPerspectiveFovRH(
        DirectX::XMConvertToRadians(20.0f),
        engine.GetSystem<Octane::WindowManager>()->GetAspectRatio(),
        0.05f,
        1000.0f);

      DirectX::XMStoreFloat4x4(
        &(cb_per_fr.ViewProjection),
        DirectX::XMMatrixTranspose(cam_view_matrix * cam_projection_matrix));
      cb_per_fr.CameraPosition = cam_position;
      cb_per_fr.LightPosition = light_position;

      render.GetD3D11Context()->UpdateSubresource(constant_buffers[1].get(), 0, nullptr, &cb_per_fr, 0, 0);
      {
        ID3D11Buffer* p_constant_buffers[1] = {constant_buffers[1].get()};
        render.GetD3D11Context()->VSSetConstantBuffers(1, 1, p_constant_buffers);
        render.GetD3D11Context()->PSSetConstantBuffers(1, 1, p_constant_buffers);
      }

      render.UseShader(phong);
      render.SetClearColor(Octane::Colors::cerulean);
      render.DrawScene();

      for (int i = 0; i < MAX_OBJECTS; ++i)
      {
        if (object_active[i])
        {
          DirectX::XMMATRIX world_matrix = DirectX::XMMatrixIdentity();
          auto scale = object_scale_rotations[i].scale;
          world_matrix *= DirectX::XMMatrixScaling(scale, scale, scale);
          world_matrix
            *= DirectX::XMMatrixRotationAxis(cam_up, DirectX::XMConvertToRadians(object_scale_rotations[i].rotation));
          auto pos = object_positions[i];
          world_matrix *= DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

          DirectX::XMStoreFloat4x4(&(cb_per_obj.World), DirectX::XMMatrixTranspose(world_matrix));
          DirectX::XMStoreFloat4x4(&(cb_per_obj.NormalWorld), DirectX::XMMatrixInverse(nullptr, world_matrix));
          cb_per_obj.Color = object_colors[i];

          render.GetD3D11Context()->UpdateSubresource(constant_buffers[0].get(), 0, nullptr, &cb_per_obj, 0, 0);
          ID3D11Buffer* p_constant_buffers[1] = {constant_buffers[0].get()};
          render.GetD3D11Context()->VSSetConstantBuffers(0, 1, p_constant_buffers);
          render.GetD3D11Context()->PSSetConstantBuffers(0, 1, p_constant_buffers);
          render.DrawMesh();
        }
      }
    }
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();

    render.Present();
  }

  _CrtDumpMemoryLeaks();
  return 0;
}
