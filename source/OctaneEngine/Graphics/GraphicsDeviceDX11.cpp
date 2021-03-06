#include <OctaneEngine/Graphics/GraphicsDeviceDX11.h>

#include <d3dcompiler.h>
#include <iostream>
#include <winrt/base.h>

#include <SDL.h>
#include <SDL_syswm.h>

#include <filesystem>

#include <OctaneEngine/FormattedOutput.h>
#include <OctaneEngine/Trace.h>

#define RCAST(x, type) reinterpret_cast<type>(x)

using namespace Octane::FormattedOutput;

namespace Octane
{

GraphicsDeviceDX11::GraphicsDeviceDX11(SDL_Window* window)
  : supported_mode_ {},
    currently_in_fullscreen_ {false},
    clear_color_ {Colors::black},
    current_mesh_ {nullptr},
    window_(window)
{
  Trace::Log(DEBUG) << "Initializing DirectX 11\n";

  HRESULT hr;
  int w, h;
  SDL_GetWindowSize(window, &w, &h);

  Trace::Log(TRACE, "Window size: [%d, %d]\n", w, h);

  supported_mode_ = DXGI_MODE_DESC {
    static_cast<UINT>(w),
    static_cast<UINT>(h),
    // TODO: get monitor refresh rate
    {60, 1},
    DXGI_FORMAT_R8G8B8A8_UNORM,
    DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
    DXGI_MODE_SCALING_UNSPECIFIED};

  SDL_SysWMinfo system_info;
  SDL_VERSION(&system_info.version);
  SDL_GetWindowWMInfo(window, &system_info);
  HWND window_handle = system_info.info.win.window;

#define SCAST(x, type) static_cast<type>(x)

  DXGI_SWAP_CHAIN_DESC swap_chain_descriptor;
  ZeroMemory(&swap_chain_descriptor, sizeof(DXGI_SWAP_CHAIN_DESC));
  swap_chain_descriptor.BufferDesc.Width = SCAST(w, UINT);
  swap_chain_descriptor.BufferDesc.Height = SCAST(h, UINT);
  swap_chain_descriptor.OutputWindow = window_handle;
  //8 bits per channel, 4 channels
  swap_chain_descriptor.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  //60hz max refresh rate for fullscreen mode
  // TODO: get monitor refresh rate
  swap_chain_descriptor.BufferDesc.RefreshRate.Numerator = 144;
  swap_chain_descriptor.BufferDesc.RefreshRate.Denominator = 1;
  //number of multisamples per pixel
  swap_chain_descriptor.SampleDesc.Count = 1;
  //antialias quality
  swap_chain_descriptor.SampleDesc.Quality = 0;
  swap_chain_descriptor.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
  //set windowed
  swap_chain_descriptor.Windowed = TRUE;
  //enable double buffering
  swap_chain_descriptor.BufferCount = 1;
  //use as render target
  swap_chain_descriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swap_chain_descriptor.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  swap_chain_descriptor.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

#if 0
  //allows switching between fullscreen and windowed mode
  swap_chain_descriptor.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#else
  swap_chain_descriptor.Flags = 0;
#endif

  if (swap_chain_descriptor.Windowed == false)
  {
    swap_chain_->SetFullscreenState(true, nullptr);
    currently_in_fullscreen_ = true;
  }
  else
  {
    currently_in_fullscreen_ = false;
  }

  hr = D3D11CreateDeviceAndSwapChain(
    nullptr,                  //let DX11 choose adapter
    D3D_DRIVER_TYPE_HARDWARE, //driver which implements d3d in hardware
    nullptr,                  //software driver
#ifdef _DEBUG
    D3D11_CREATE_DEVICE_DEBUG, //runtime debug layers
#endif
#ifndef _DEBUG
    0,
#endif
    nullptr,                //feature levels
    0,                      //length of feature levels array
    D3D11_SDK_VERSION,      //DirectX11 SDK
    &swap_chain_descriptor, //swap chain info
    swap_chain_.put(),
    device_.put(),
    nullptr, //supported feature levels
    device_context_.put());
  assert(SUCCEEDED(hr));

  CreateRenderTarget();
  CreateDepthBuffer();
  CreateSamplerState();
  CreateConstantBuffers();
  CreateRasterizerState();
#if 0
  winrt::com_ptr<ID3D11Texture2D> back_buffer;
  hr = swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), back_buffer.put_void());
  assert(SUCCEEDED(hr));
  hr = device_->CreateRenderTargetView(back_buffer.get(), nullptr, render_target_view_.put());
  assert(SUCCEEDED(hr));
  D3D11_TEXTURE2D_DESC back_buffer_descriptor = {0};
  back_buffer->GetDesc(&back_buffer_descriptor);

  D3D11_DEPTH_STENCIL_DESC depth_stencil_descriptor {true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS_EQUAL};

  hr = device_->CreateDepthStencilState(&depth_stencil_descriptor, depth_stencil_state_.put());
  assert(SUCCEEDED(hr));
  device_context_->OMSetDepthStencilState(depth_stencil_state_.get(), 1);

  D3D11_TEXTURE2D_DESC depth_stencil_buffer_descriptor {
    back_buffer_descriptor.Width,
    back_buffer_descriptor.Height,
    1,
    1,
    DXGI_FORMAT_D24_UNORM_S8_UINT,
    back_buffer_descriptor.SampleDesc,
    D3D11_USAGE_DEFAULT,
    D3D11_BIND_DEPTH_STENCIL,
    0,
    0,
  };

  hr = device_->CreateTexture2D(&depth_stencil_buffer_descriptor, nullptr, depth_stencil_buffer.put());

  D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_descriptor {
    depth_stencil_buffer_descriptor.Format,
    D3D11_DSV_DIMENSION_TEXTURE2D,
    0,
  };
  depth_stencil_view_descriptor.Texture2D.MipSlice = 0;

  device_->CreateDepthStencilView(
    depth_stencil_buffer.get(),
    &depth_stencil_view_descriptor,
    depth_stencil_view_.put());

  {
    auto* p_render_target_view = render_target_view_.get();
    device_context_->OMSetRenderTargets(1, &p_render_target_view, depth_stencil_view_.get());
  }
#endif

#if 0
  D3D11_RASTERIZER_DESC rasterizer_descriptor {
    D3D11_FILL_SOLID,
    D3D11_CULL_BACK,
    true,
  };

  hr = device_->CreateRasterizerState(&rasterizer_descriptor, rasterizer_state_.put());
  assert(SUCCEEDED(hr));
  device_context_->RSSetState(rasterizer_state_.get());
#endif

#if 0
  {
    D3D11_BUFFER_DESC
    cb_buffer_descriptor {
      sizeof(ShaderConstantBuffers::PerObjectConstants::RawData),
      D3D11_USAGE_DEFAULT,
      D3D11_BIND_CONSTANT_BUFFER,
      0,
      0,
      0};
    hr = GetD3D11Device()->CreateBuffer(&cb_buffer_descriptor, nullptr, constant_buffers_[0].put());
    assert(SUCCEEDED(hr));
  }

  {
    D3D11_BUFFER_DESC
    cb_buffer_descriptor {
      sizeof(ShaderConstantBuffers::PerFrameConstants::RawData),
      D3D11_USAGE_DEFAULT,
      D3D11_BIND_CONSTANT_BUFFER,
      0,
      0,
      0};
    hr = GetD3D11Device()->CreateBuffer(&cb_buffer_descriptor, nullptr, constant_buffers_[1].put());
    assert(SUCCEEDED(hr));
  }
#endif
////Screen Modes
//DXGI_FORMAT desiredColorFormat;       //the desired color format
//unsigned int numberOfSupportedModes;  //The number of supported screen
//DXGI_MODE_DESC* supportedModes;       //list of all supported screen
//DXGI_MODE_DESC currentModeDescription; //description of the current mode
//unsigned int currentModeIndex;         // the index of the current mode in the list of all supported screen modes
//bool startInFullscreen;                // true iff the game should start in fullscreen mode
//BOOL currentlyInFullscreen;            // true iff the game is currently in fullscreen mode
//bool changeMode;                       // true iff the screen resolution should be changed this frame

//// functions to change screen resolutions
//void changeResolution(
//  bool
//    increase); // changes the screen resolution, if increase is true, a higher resolution is chosen, else the resolution is lowered; returns true iff the screen resolution should be changed

// set up the default viewport to match the window
#if 0
  RECT window_rect;
  GetClientRect(window_handle, &window_rect);
  D3D11_VIEWPORT viewport {
    0.0f,
    0.0f,
    (FLOAT)(window_rect.right - window_rect.left),
    (FLOAT)(window_rect.bottom - window_rect.top),
    0.0f,
    1.0f};
  device_context_->RSSetViewports(1, &viewport);
#endif
}

GraphicsDeviceDX11::~GraphicsDeviceDX11()
{
  swap_chain_.attach(nullptr);
  swap_chain_->SetFullscreenState(false, nullptr);
  device_.attach(nullptr);
  device_context_.attach(nullptr);
}

void GraphicsDeviceDX11::ClearScreen()
{
  {
    auto* p_render_target_view = render_target_view_.get();
    device_context_->OMSetRenderTargets(1, &p_render_target_view, depth_stencil_view_.get());
  }
  ClearBuffers();
}

void GraphicsDeviceDX11::SetClearColor(Color c)
{
  clear_color_ = c;
}

void GraphicsDeviceDX11::Present()
{
  swap_chain_->Present(1, 0);
}

void GraphicsDeviceDX11::ResizeFramebuffer(SDL_Window* window)
{
  WindowResized();
#if 0
  //check for fullscreen switch
  BOOL want_full_screen = false;
  swap_chain_->GetFullscreenState(&want_full_screen, NULL);

  // we don't set size here since we'll do that with SDL elsewhere
  if (currently_in_fullscreen_ == !want_full_screen)
  {
    swap_chain_->SetFullscreenState(want_full_screen, nullptr);
    currently_in_fullscreen_ = want_full_screen;
  }

  // for some reason .Release() is not defined so we'll just attach nullptr instead...
  //render_target_view_.attach(nullptr);
  if(render_target_view_.get())
  {
    device_context_->OMSetRenderTargets(0, 0, 0);
    render_target_view_.detach();
  }
  //get rid of old depth buffer
  depth_stencil_view_.attach(nullptr);
  depth_stencil_buffer.attach(nullptr);


  // passing 0 for width/height gets automatic size
  HRESULT hr = swap_chain_->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, /* DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH */ 0);
  assert(SUCCEEDED(hr));

  ID3D11Texture2D* d3d11_frame_buffer;
  hr = swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&d3d11_frame_buffer);
  assert(SUCCEEDED(hr));

  hr = device_->CreateRenderTargetView(d3d11_frame_buffer, nullptr, render_target_view_.put());
  assert(SUCCEEDED(hr));
  d3d11_frame_buffer.attach(nullptr);

  // set up the default viewport to match the window
  int width;
  int height;
  {
    SDL_GetWindowSize(window, &width, &height);

    D3D11_VIEWPORT viewport {0.0f, 0.0f, (FLOAT)(width), (FLOAT)(height), 0.0f, 1.0f};
    device_context_->RSSetViewports(1, &viewport);
  }

    //create new depth buffer
  {
    //set up Depth/Stencil buffer
    D3D11_TEXTURE2D_DESC depthStencilDesc;
    depthStencilDesc.Width = width;
    depthStencilDesc.Height = height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    D3D11_DEPTH_STENCIL_DESC dsDesc;

    // Depth test parameters
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    // Stencil test parameters
    dsDesc.StencilEnable = true;
    dsDesc.StencilReadMask = 255;
    dsDesc.StencilWriteMask = 255;

    // Stencil operations if pixel is front-facing
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create depth stencil state
    auto dssPtr = depth_stencil_state_.get();
    device_->CreateDepthStencilState(&dsDesc, &dssPtr);


    HRESULT hr = device_->CreateTexture2D(&depthStencilDesc, nullptr, depth_stencil_buffer.put());

    if (FAILED(hr))
    {
      Trace::Log(Severity::ERROR, "Error creating depth stencil buffer");
    }

    hr = device_->CreateDepthStencilView(depth_stencil_buffer.get(), nullptr, depth_stencil_view_.put());

    if (FAILED(hr))
    {
      Trace::Log(Severity::ERROR, "Error creating depth stencil view");
    }
    device_context_->OMSetDepthStencilState(dssPtr, 0);
  }
#endif
}

Shader GraphicsDeviceDX11::CreateShader(LPCWSTR shader_path, int input_layout)
{
  Trace::Assert(
    std::filesystem::exists(shader_path),
    SERIALIZATION,
    "shader file %s%ls%s loaded successfully.",
    Set(Yellow).c_str(),
    shader_path,
    Set().c_str());

  HRESULT hr;
  ID3DBlob* error_buffer = nullptr;

  UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
  flags |= D3DCOMPILE_DEBUG;
#endif

  Shader shader;

  // Compile vertex shader
  Trace::Log(DEBUG, "Compiling vertex shader %s%ls%s.\n", Set(Yellow).c_str(), shader_path, Set().c_str());
  hr = D3DCompileFromFile(
    shader_path,
    nullptr,
    D3D_COMPILE_STANDARD_FILE_INCLUDE,
    "vs_main",
    "vs_5_0",
    flags,
    0,
    shader.vertex_shader_buffer_.put(),
    &error_buffer);

  if (FAILED(hr))
  {
    if (error_buffer)
    {
      std::clog << ((char*)error_buffer->GetBufferPointer());
      error_buffer->Release();
    }
    if (shader.vertex_shader_buffer_)
    {
      shader.vertex_shader_buffer_.attach(nullptr);
    }
    assert(!"Couldn't compile vertex shader");
  }

  // Compile pixel shader
  Trace::Log(DEBUG, "Compiling pixel shader %s%ls%s.\n", Set(Yellow).c_str(), shader_path, Set().c_str());
  hr = D3DCompileFromFile(
    shader_path,
    nullptr,
    D3D_COMPILE_STANDARD_FILE_INCLUDE,
    "ps_main",
    "ps_5_0",
    flags,
    0,
    shader.pixel_shader_buffer_.put(),
    &error_buffer);

  if (FAILED(hr))
  {
    if (error_buffer)
    {
      OutputDebugStringA((char*)error_buffer->GetBufferPointer());
      error_buffer->Release();
    }
    if (shader.pixel_shader_buffer_)
    {
      shader.pixel_shader_buffer_.attach(nullptr);
    }
    assert(!"Couldn't compile pixel shader");
  }

  hr = GetD3D11Device()->CreateVertexShader(
    shader.vertex_shader_buffer_->GetBufferPointer(),
    shader.vertex_shader_buffer_->GetBufferSize(),
    nullptr,
    shader.vertex_shader_.put());
  assert(SUCCEEDED(hr));

  hr = GetD3D11Device()->CreatePixelShader(
    shader.pixel_shader_buffer_->GetBufferPointer(),
    shader.pixel_shader_buffer_->GetBufferSize(),
    nullptr,
    shader.pixel_shader_.put());
  assert(SUCCEEDED(hr));

  D3D11_INPUT_ELEMENT_DESC input_element_descriptor[Shader::InputLayout_COUNT] {};
  int input_element_count = 0;
  for (int i = 0; i < Shader::InputLayout_COUNT; ++i)
  {
    if (input_layout & 1 << i)
    {
      ++input_element_count;
    }
  }
  assert(input_element_count > 0);

  int elements_filled = 0;
  if (input_layout & Shader::InputLayout_POS)
  {
    input_element_descriptor[elements_filled] = {
      "POS",
      0,
      DXGI_FORMAT_R32G32B32_FLOAT,
      0,
      elements_filled ? D3D11_APPEND_ALIGNED_ELEMENT : 0,
      D3D11_INPUT_PER_VERTEX_DATA,
      0};
    ++elements_filled;
  }
  if (input_layout & Shader::InputLayout_COL)
  {
    input_element_descriptor[elements_filled] = {
      "COL",
      0,
      DXGI_FORMAT_R32G32B32_FLOAT,
      0,
      elements_filled ? D3D11_APPEND_ALIGNED_ELEMENT : 0,
      D3D11_INPUT_PER_VERTEX_DATA,
      0};
    ++elements_filled;
  }
  if (input_layout & Shader::InputLayout_NOR)
  {
    input_element_descriptor[elements_filled] = {
      "NOR",
      0,
      DXGI_FORMAT_R32G32B32_FLOAT,
      0,
      elements_filled ? D3D11_APPEND_ALIGNED_ELEMENT : 0,
      D3D11_INPUT_PER_VERTEX_DATA,
      0};
    ++elements_filled;
  }
  if (input_layout & Shader::InputLayout_TEX)
  {
    input_element_descriptor[elements_filled] = {
      "TEX",
      0,
      DXGI_FORMAT_R32G32_FLOAT,
      0,
      elements_filled ? D3D11_APPEND_ALIGNED_ELEMENT : 0,
      D3D11_INPUT_PER_VERTEX_DATA,
      0};
    ++elements_filled;
  }

  hr = GetD3D11Device()->CreateInputLayout(
    input_element_descriptor,
    input_element_count,
    shader.vertex_shader_buffer_->GetBufferPointer(),
    shader.vertex_shader_buffer_->GetBufferSize(),
    shader.vertex_shader_input_layout_.put());
  assert(SUCCEEDED(hr));

  return shader;
}

void GraphicsDeviceDX11::UseShader(Shader& shader)
{
  GetD3D11Context()->IASetInputLayout(shader.GetInputLayout());
  GetD3D11Context()->VSSetShader(shader.vertex_shader_.get(), nullptr, 0);
  GetD3D11Context()->PSSetShader(shader.pixel_shader_.get(), nullptr, 0);
}

void GraphicsDeviceDX11::EmplaceMesh(eastl::hash_map<Mesh_Key, MeshPtr>& meshes, Mesh_Key placement, Mesh const& mesh)
  const
{
  if (mesh.index_buffer.empty() || mesh.vertex_buffer.empty())
    return;
  meshes[placement] = MeshPtr(new MeshDX11 {
    sizeof(Mesh::Vertex),
    mesh.vertex_buffer.size(),
    mesh.index_buffer.size(),
    mesh.textures,
    mesh.material});
  auto& newMesh = meshes.find(placement)->second;
  //new (placement) MeshDX11({sizeof(Mesh::Vertex), mesh.vertex_buffer.size(), mesh.index_buffer.size()});
  HRESULT hr;

  {
    D3D11_BUFFER_DESC vertex_buffer_descriptor {
      static_cast<UINT>(sizeof(Mesh::Vertex) * mesh.vertex_buffer.size()),
      D3D11_USAGE_DEFAULT,
      D3D11_BIND_VERTEX_BUFFER,
      0,
      0,
      0};
    D3D11_SUBRESOURCE_DATA subresource_data {mesh.vertex_buffer.data(), 0, 0};

    hr = GetD3D11Device()->CreateBuffer(&vertex_buffer_descriptor, &subresource_data, newMesh->vertex_buffer_.put());
    assert(SUCCEEDED(hr));
  }

  {
    D3D11_BUFFER_DESC index_buffer_descriptor {
      static_cast<UINT>(sizeof(Mesh::Index) * mesh.index_buffer.size()),
      D3D11_USAGE_DEFAULT,
      D3D11_BIND_INDEX_BUFFER,
      0,
      0,
      0};
    D3D11_SUBRESOURCE_DATA subresource_data {mesh.index_buffer.data(), 0, 0};

    hr = GetD3D11Device()->CreateBuffer(&index_buffer_descriptor, &subresource_data, newMesh->index_buffer_.put());
    assert(SUCCEEDED(hr));
  }
}

void GraphicsDeviceDX11::UseMesh(MeshDX11 const& mesh)
{
  UINT const vertex_stride = static_cast<UINT>(mesh.vertex_size_);
  UINT const vertex_offset = 0;

  ID3D11Buffer* buffer = mesh.vertex_buffer_.get();
  GetD3D11Context()->IASetVertexBuffers(0, 1, &buffer, &vertex_stride, &vertex_offset);
  GetD3D11Context()->IASetIndexBuffer(mesh.index_buffer_.get(), DXGI_FORMAT_R32_UINT, 0);

  current_mesh_ = &mesh;
}

void GraphicsDeviceDX11::DrawMesh()
{
  if (current_mesh_->index_count_)
  {
    GetD3D11Context()->DrawIndexed(static_cast<UINT>(current_mesh_->index_count_), 0, 0);
  }
  else // Not an indexed mesh
  {
    GetD3D11Context()->Draw(static_cast<UINT>(current_mesh_->vertex_count_), 0);
  }
}

ID3D11Device* GraphicsDeviceDX11::GetD3D11Device() const
{
  return device_.get();
}

ID3D11DeviceContext* GraphicsDeviceDX11::GetD3D11Context() const
{
  return device_context_.get();
}

void GraphicsDeviceDX11::SetWireframeMode(bool enable)
{
  D3D11_RASTERIZER_DESC rasterizer_descriptor {
    (enable ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID),
    (enable ? D3D11_CULL_NONE : D3D11_CULL_BACK),
    true,
  };

  rasterizer_state_ = nullptr;
  GetD3D11Device()->CreateRasterizerState(&rasterizer_descriptor, rasterizer_state_.put());
  GetD3D11Context()->RSSetState(rasterizer_state_.get());
}

void GraphicsDeviceDX11::ClearBuffers()
{
  DirectX::XMFLOAT4 clear_color {clear_color_.r, clear_color_.g, clear_color_.b, 1.0};
  device_context_->ClearRenderTargetView(render_target_view_.get(), &(clear_color.x));
  device_context_->ClearDepthStencilView(depth_stencil_view_.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void GraphicsDeviceDX11::CreateSamplerState()
{
  D3D11_SAMPLER_DESC samplerdesc;
  ZeroMemory(&samplerdesc, sizeof(D3D11_SAMPLER_DESC));
  samplerdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  samplerdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerdesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  samplerdesc.MinLOD = 0;
  samplerdesc.MaxLOD = D3D11_FLOAT32_MAX;
  HRESULT hr = device_->CreateSamplerState(&samplerdesc, sampler_state_.put());
  if (FAILED(hr))
  {
    Trace::Log(Severity::ERROR, "failed to create a sampler state");
  }
  auto ssPtr = sampler_state_.get();
  device_context_->PSSetSamplers(0, 1, &ssPtr);
}

void GraphicsDeviceDX11::CleanupRenderTarget()
{
  if (render_target_view_.get())
  {
    device_context_->OMSetRenderTargets(0, 0, 0);
    render_target_view_.attach(nullptr);
  }
}

void GraphicsDeviceDX11::CreateRenderTarget()
{
  HRESULT hr = swap_chain_->ResizeBuffers(0, 0, 0, DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, 0);

  if (FAILED(hr))
  {
    Trace::Log(Severity::FAILURE, "failed to resize swapchain buffers");
  }
  //get backbuffer from swap chain
  winrt::com_ptr<ID3D11Texture2D> backBuffer;
  hr = swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), RCAST(backBuffer.put(), void**));

  if (FAILED(hr))
  {
    Trace::Log(Severity::FAILURE, "IDXGISwapChain::GetBuffer failed");
    throw; //exit immediately
  }

  hr = device_->CreateRenderTargetView(backBuffer.get(), nullptr, render_target_view_.put());
  if (FAILED(hr))
  {
    Trace::Log(Severity::FAILURE, "failed to create render target view");
    throw; //exit immediately
  }
  SetupViewport();
}

void GraphicsDeviceDX11::SetupViewport()
{
  RECT clientRect;
  //set up viewport
  int w, h;
  SDL_GetWindowSize(window_, &w, &h);

  viewport.Width = w;
  viewport.Height = h;
  viewport.TopLeftX = 0;
  viewport.TopLeftY = 0;
  viewport.MinDepth = 0;
  viewport.MaxDepth = 1;
  device_context_->RSSetViewports(1, &viewport);
}

void GraphicsDeviceDX11::WindowResized()
{
  SetupViewport();
  CleanupRenderTarget();
  DeleteDepthBuffer();
  CreateDepthBuffer();
  CreateRenderTarget();

  // the windows api is not const-correct. it won't modify this,
  // but we still need a non-const pointer as our 1-element array
  auto* p_render_target_view = render_target_view_.get();
  device_context_->OMSetRenderTargets(1, &p_render_target_view, depth_stencil_view_.get());
  assert(p_render_target_view == render_target_view_.get()); // the pointer should be unchanged
}

void GraphicsDeviceDX11::CreateDepthBuffer()
{
  //set up Depth/Stencil buffer
  D3D11_TEXTURE2D_DESC depthStencilDesc;
  depthStencilDesc.Width = 2048;
  depthStencilDesc.Height = 2048;
  depthStencilDesc.MipLevels = 1;
  depthStencilDesc.ArraySize = 1;
  depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthStencilDesc.SampleDesc.Count = 1;
  depthStencilDesc.SampleDesc.Quality = 0;
  depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
  depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depthStencilDesc.CPUAccessFlags = 0;
  depthStencilDesc.MiscFlags = 0;

  D3D11_DEPTH_STENCIL_DESC dsDesc;

  // Depth test parameters
  dsDesc.DepthEnable = true;
  dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

  // Stencil test parameters
  dsDesc.StencilEnable = true;
  dsDesc.StencilReadMask = 255;
  dsDesc.StencilWriteMask = 255;

  // Stencil operations if pixel is front-facing
  dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
  dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  // Stencil operations if pixel is back-facing
  dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
  dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  // Create depth stencil state
  device_->CreateDepthStencilState(&dsDesc, depth_stencil_state_.put());

  HRESULT hr = device_->CreateTexture2D(&depthStencilDesc, nullptr, depth_stencil_buffer.put());

  if (FAILED(hr))
  {
    Trace::Log(Severity::FAILURE, "Error creating depth stencil buffer");
  }

  hr = device_->CreateDepthStencilView(depth_stencil_buffer.get(), nullptr, depth_stencil_view_.put());

  if (FAILED(hr))
  {
    Trace::Log(Severity::FAILURE, "Error creating depth stencil view");
  }
  device_context_->OMSetDepthStencilState(depth_stencil_state_.get(), 0);
}

void GraphicsDeviceDX11::DeleteDepthBuffer()
{
  depth_stencil_view_.attach(nullptr);
  depth_stencil_buffer.attach(nullptr);
  depth_stencil_state_.attach(nullptr);
}

void GraphicsDeviceDX11::CreateConstantBuffers()
{
  D3D11_BUFFER_DESC
  cb_buffer_descriptor {
    sizeof(ShaderConstantBuffers::PerObjectConstants::RawData),
    D3D11_USAGE_DEFAULT,
    D3D11_BIND_CONSTANT_BUFFER,
    0,
    0,
    0};
  HRESULT hr = GetD3D11Device()->CreateBuffer(&cb_buffer_descriptor, nullptr, constant_buffers_[0].put());
  assert(SUCCEEDED(hr));

  {
    D3D11_BUFFER_DESC
    cb_buffer_descriptor {
      sizeof(ShaderConstantBuffers::PerFrameConstants::RawData),
      D3D11_USAGE_DEFAULT,
      D3D11_BIND_CONSTANT_BUFFER,
      0,
      0,
      0};
    hr = GetD3D11Device()->CreateBuffer(&cb_buffer_descriptor, nullptr, constant_buffers_[1].put());
    assert(SUCCEEDED(hr));
  }
}

void GraphicsDeviceDX11::CreateRasterizerState()
{
  D3D11_RASTERIZER_DESC rasterizer_descriptor {
    D3D11_FILL_SOLID,
    D3D11_CULL_BACK,
    true,
  };

  HRESULT hr = device_->CreateRasterizerState(&rasterizer_descriptor, rasterizer_state_.put());
  assert(SUCCEEDED(hr));
  device_context_->RSSetState(rasterizer_state_.get());
}

} // namespace Octane
