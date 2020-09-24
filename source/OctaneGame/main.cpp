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
int window_width = 1280;
int window_height = 720;
SDL_Window* window = nullptr;

winrt::com_ptr<ID3D11Buffer> model_vertex_buffer;
winrt::com_ptr<ID3D11Buffer> model_index_buffer;
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

  Octane::Engine engine;
  engine.AddSystem(new Octane::FramerateController {});
  engine.AddSystem(new Octane::InputHandler {&engine});

  std::clog << "[== Project Octane ==]\n";
  std::clog << "Initializing SDL\n";
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
  {
    std::clog << "SDL could not initialize! SDL_Error:" << SDL_GetError() << "\n";
    __debugbreak();
    return EXIT_FAILURE;
  }

  //Create window
  window = SDL_CreateWindow(
    "Project Octane",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    window_width,
    window_height,
    SDL_WINDOW_SHOWN);
  if (window == nullptr)
  {
    std::clog << "Window could not be created! SDL_Error:" << SDL_GetError() << "\n";
    __debugbreak();
    return EXIT_FAILURE;
  }

  std::clog << "Initializing DX11\n";
  Octane::RenderDX11 render {window};
  Octane::Shader phong = render.CreateShader(
    L"assets/shaders/phong.hlsl",
    Octane::Shader::InputLayout_POS | Octane::Shader::InputLayout_NOR);
  render.SetClearColor(Octane::Colors::cerulean);

  Octane::OBJParser obj_parser;
  Octane::Mesh m = obj_parser.ParseOBJ(L"assets/models/cube_rounded.obj");

  UINT vertex_stride = sizeof(Octane::Mesh::Vertex);
  UINT vertex_offset = 0;

  {
    D3D11_BUFFER_DESC vertex_buffer_descriptor {
      sizeof(Octane::Mesh::Vertex) * m.vertex_count,
      D3D11_USAGE_DEFAULT,
      D3D11_BIND_VERTEX_BUFFER,
      0,
      0,
      0};
    D3D11_SUBRESOURCE_DATA subresource_data {m.vertex_buffer, 0, 0};

    HRESULT hr
      = render.GetD3D11Device()->CreateBuffer(&vertex_buffer_descriptor, &subresource_data, model_vertex_buffer.put());
    assert(SUCCEEDED(hr));
  }

  {
    D3D11_BUFFER_DESC index_buffer_descriptor {
      sizeof(Octane::Mesh::Index) * m.index_count,
      D3D11_USAGE_DEFAULT,
      D3D11_BIND_INDEX_BUFFER,
      0,
      0,
      0};
    D3D11_SUBRESOURCE_DATA subresource_data {m.index_buffer, 0, 0};

    HRESULT hr
      = render.GetD3D11Device()->CreateBuffer(&index_buffer_descriptor, &subresource_data, model_index_buffer.put());
    assert(SUCCEEDED(hr));
  }

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
  render.GetD3D11Context()->IASetInputLayout(phong.GetInputLayout());
  ID3D11Buffer* p_model_vertex_buffer = model_vertex_buffer.get();
  render.GetD3D11Context()->IASetVertexBuffers(0, 1, &p_model_vertex_buffer, &vertex_stride, &vertex_offset);
  render.GetD3D11Context()->IASetIndexBuffer(model_index_buffer.get(), DXGI_FORMAT_R32_UINT, 0);

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

  ImGui_ImplSDL2_InitForD3D(window);
  ImGui_ImplDX11_Init(render.GetD3D11Device(), render.GetD3D11Context());

  bool scene_settings_open = false;
  bool demo_window_open = false;

  while (!engine.ShouldQuit())
  {
    engine.Update();

    // This is extremely clean
    auto* frc = engine.GetSystem<Octane::FramerateController>();
    float dt = frc->GetDeltaTime();

    auto* input = engine.GetSystem<Octane::InputHandler>();

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("File"))
      {
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

    DirectX::XMStoreFloat3(&cam_velocity, DirectX::XMVectorScale(DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&cam_velocity)), 0.5f));

    cam_position.x += cam_velocity.x;
    cam_position.y += cam_velocity.y;
    cam_position.z += cam_velocity.z;
      
      cam_view_matrix
      = DirectX::XMMatrixLookAtRH(DirectX::XMLoadFloat3(&cam_position), DirectX::XMLoadFloat3(&cam_target), cam_up);
    cam_projection_matrix = DirectX::XMMatrixPerspectiveFovRH(
      DirectX::XMConvertToRadians(20.0f),
      (float)window_width / (float)window_height,
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
        render.GetD3D11Context()->DrawIndexed(m.index_count, 0, 0);
      }
    }

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();

    render.Present();
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  _CrtDumpMemoryLeaks();
  return 0;
}
