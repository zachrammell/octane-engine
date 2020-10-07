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

#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/FramerateController.h>
#include <OctaneEngine/InputHandler.h>
#include <OctaneEngine/Physics/World.h>
#include <OctaneEngine/SceneSys.h>
#include <OctaneEngine/WindowManager.h>
#include <OctaneEngine/Physics/Box.h>

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
  // pass-through to standard memory manager for now, will later be a custom one
  return new uint8_t[size];
}

namespace
{
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
  engine.AddSystem(new Octane::EntitySys {&engine});
  engine.AddSystem(new Octane::ComponentSys {&engine});
  engine.AddSystem(new Octane::SceneSys {&engine});

  std::clog << "Initializing DX11\n";
  Octane::RenderDX11 render {engine.GetSystem<Octane::WindowManager>()->GetHandle()};
  Octane::Shader phong = render.CreateShader(
    L"assets/shaders/phong.hlsl",
    Octane::Shader::InputLayout_POS | Octane::Shader::InputLayout_NOR);
  render.SetClearColor(Octane::Colors::cerulean);

  Octane::OBJParser obj_parser;
  Octane::Mesh sphere_mesh = obj_parser.ParseOBJ(L"assets/models/sphere.obj");
  Octane::MeshDX11 sphere_mesh_dx11 = render.CreateMesh(sphere_mesh);
  Octane::Mesh cube_mesh = obj_parser.ParseOBJ(L"assets/models/cube_rounded.obj");
  Octane::MeshDX11 cube_mesh_dx11 = render.CreateMesh(cube_mesh);

  render.GetD3D11Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  render.UseMesh(sphere_mesh_dx11);

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

  object_active[101] = true;
  object_colors[101] = Octane::Colors::blue;
  object_scale_rotations[101].scale = 0.25f;

  auto body_100 = world->AddRigidBody();
  DirectX::XMFLOAT3 constraints = {1.0f, 1.0f, 1.0f};
  body_100->SetLinearConstraints(constraints);
  body_100->SetAngularConstraints(constraints);
  auto prim_100 = world->AddPrimitive(body_100, Octane::ePrimitiveType::Box);
  static_cast<Octane::Box*>(prim_100)->SetBox(0.25f, 0.25f, 0.25f);

  auto body_101 = world->AddRigidBody();
  body_101->SetLinearConstraints(constraints);
  body_101->SetAngularConstraints(constraints);
  auto prim_101 = world->AddPrimitive(body_101, Octane::ePrimitiveType::Ellipsoid);

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
        render.ClearScreen();
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

      DirectX::XMFLOAT3 sample_force2;
      sample_force2.x = 1.0f * (input->KeyHeld(SDLK_f) - input->KeyHeld(SDLK_h));
      sample_force2.y = 1.0f * (input->KeyHeld(SDLK_t) - input->KeyHeld(SDLK_g));
      sample_force2.z = 1.0f * (input->KeyHeld(SDLK_r) - input->KeyHeld(SDLK_y));

      body_100->ApplyForceCentroid(sample_force);
      body_100->SyncToPosition(object_positions[100]);
      body_101->ApplyForceCentroid(sample_force2);
      body_101->SyncToPosition(object_positions[101]);

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

      render.ShaderConstants()
        .PerFrame()
        .SetViewProjection(DirectX::XMMatrixTranspose(cam_view_matrix * cam_projection_matrix))
        .SetCameraPosition(cam_position)
        .SetLightPosition(light_position);

      render.Upload(render.ShaderConstants().PerFrame());

      render.UseShader(phong);
      render.SetClearColor(Octane::Colors::cerulean);
      render.ClearScreen();

      for (int i = 0; i < MAX_OBJECTS; ++i)
      {
        if (object_active[i])
        {
          DirectX::XMMATRIX object_world_matrix = DirectX::XMMatrixIdentity();
          auto scale = object_scale_rotations[i].scale;
          object_world_matrix *= DirectX::XMMatrixScaling(scale, scale, scale);
          object_world_matrix
            *= DirectX::XMMatrixRotationAxis(cam_up, DirectX::XMConvertToRadians(object_scale_rotations[i].rotation));
          auto pos = object_positions[i];
          object_world_matrix *= DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
          if (i % 2 == 0)
          {
            render.UseMesh(cube_mesh_dx11);
          }
          else
          {
            render.UseMesh(sphere_mesh_dx11);
          }
          render.ShaderConstants()
            .PerObject()
            .SetWorldMatrix(DirectX::XMMatrixTranspose(object_world_matrix))
            .SetWorldNormalMatrix(DirectX::XMMatrixInverse(nullptr, object_world_matrix))
            .SetColor(object_colors[i]);
          render.Upload(render.ShaderConstants().PerObject());
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
  delete body_100;
  delete body_101;
  delete prim_100;
  delete prim_101;

  _CrtDumpMemoryLeaks();
  return 0;
}
