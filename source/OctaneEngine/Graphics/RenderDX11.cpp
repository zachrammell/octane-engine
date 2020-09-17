#include <OctaneEngine/Graphics/RenderDX11.h>

#include <d3dcompiler.h>
#include <iostream>
#include <winrt/base.h>

#include <SDL.h>
#include <SDL_syswm.h>


namespace Octane
{

RenderDX11::RenderDX11(SDL_Window* window)
  : clear_color_{ Colors::black }
{
  HRESULT hr;
  int w, h;
  SDL_GetWindowSize(window, &w, &h);
  DXGI_MODE_DESC buffer_description
  {
    static_cast<UINT>(w),
    static_cast<UINT>(h),
    // TODO: get monitor refresh rate
    {60, 1},
    DXGI_FORMAT_R8G8B8A8_UNORM,
    DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
    DXGI_MODE_SCALING_UNSPECIFIED
  };

  SDL_SysWMinfo system_info;
  SDL_VERSION(&system_info.version);
  SDL_GetWindowWMInfo(window, &system_info);
  HWND window_handle = system_info.info.win.window;
  DXGI_SWAP_CHAIN_DESC swap_chain_descriptor
  {
    buffer_description,
    DXGI_SAMPLE_DESC{1, 0},
    DXGI_USAGE_RENDER_TARGET_OUTPUT,
    1,
    window_handle,
    true,
    DXGI_SWAP_EFFECT_DISCARD,
    0
  };

  hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
    &swap_chain_descriptor, swap_chain_.put(), device_.put(), nullptr, device_context_.put());
  assert(SUCCEEDED(hr));

  winrt::com_ptr<ID3D11Texture2D> back_buffer;
  hr = swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), back_buffer.put_void());
  assert(SUCCEEDED(hr));
  hr = device_->CreateRenderTargetView(back_buffer.get(), nullptr, render_target_view_.put());
  assert(SUCCEEDED(hr));
  D3D11_TEXTURE2D_DESC back_buffer_descriptor = { 0 };
  back_buffer->GetDesc(&back_buffer_descriptor);

  D3D11_DEPTH_STENCIL_DESC depth_stencil_descriptor
  {
    true,
    D3D11_DEPTH_WRITE_MASK_ALL,
    D3D11_COMPARISON_LESS
  };

  hr = device_->CreateDepthStencilState(&depth_stencil_descriptor, depth_stencil_state_.put());
  assert(SUCCEEDED(hr));
  device_context_->OMSetDepthStencilState(depth_stencil_state_.get(), 1);

  D3D11_TEXTURE2D_DESC depth_stencil_buffer_descriptor
  {
    back_buffer_descriptor.Width,
    back_buffer_descriptor.Height,
    1,
    1,
    DXGI_FORMAT_D24_UNORM_S8_UINT,
    back_buffer_descriptor.SampleDesc,
    D3D11_USAGE_DEFAULT,
    D3D11_BIND_DEPTH_STENCIL,
    0,
    0
  };

  winrt::com_ptr<ID3D11Texture2D> depth_stencil_buffer;
  hr = device_->CreateTexture2D(
    &depth_stencil_buffer_descriptor,
    nullptr,
    depth_stencil_buffer.put()
  );

  D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_descriptor
  {
    depth_stencil_buffer_descriptor.Format,
    D3D11_DSV_DIMENSION_TEXTURE2D,
    0,
  };
  depth_stencil_view_descriptor.Texture2D.MipSlice = 0;

  device_->CreateDepthStencilView(
    depth_stencil_buffer.get(),
    &depth_stencil_view_descriptor,
    depth_stencil_view_.put()
  );

  {
  auto* p_render_target_view = render_target_view_.get();
  device_context_->OMSetRenderTargets(1, &p_render_target_view, depth_stencil_view_.get());
  }

  D3D11_RASTERIZER_DESC rasterizer_descriptor
  {
    D3D11_FILL_SOLID,
    D3D11_CULL_BACK,
    true,

  };

  hr = device_->CreateRasterizerState(&rasterizer_descriptor, rasterizer_state_.put());
  assert(SUCCEEDED(hr));
  device_context_->RSSetState(rasterizer_state_.get());

  // set up the default viewport to match the window
  {
    RECT window_rect;
    GetClientRect(window_handle, &window_rect);
    D3D11_VIEWPORT viewport
    {
      0.0f,
      0.0f,
      (FLOAT)(window_rect.right - window_rect.left),
      (FLOAT)(window_rect.bottom - window_rect.top),
      0.0f,
      1.0f
    };
    device_context_->RSSetViewports(1, &viewport);
  }
}

RenderDX11::~RenderDX11()
{
  swap_chain_->Release();
  device_->Release();
  device_context_->Release();
}

void RenderDX11::DrawScene()
{
  {
  auto* p_render_target_view = render_target_view_.get();
  device_context_->OMSetRenderTargets(1, &p_render_target_view, depth_stencil_view_.get());
  }
  ClearBuffers();
}

void RenderDX11::SetClearColor(Color c)
{
  clear_color_ = c;
}

void RenderDX11::Present()
{
  swap_chain_->Present(1, 0);
}

void RenderDX11::ResizeFramebuffer(SDL_Window* window)
{
  device_context_->OMSetRenderTargets(0, 0, 0);
  render_target_view_->Release();

  HRESULT hr = swap_chain_->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
  assert(SUCCEEDED(hr));

  ID3D11Texture2D* d3d11_frame_buffer;
  hr = swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&d3d11_frame_buffer);
  assert(SUCCEEDED(hr));

  hr = device_->CreateRenderTargetView(d3d11_frame_buffer, nullptr, render_target_view_.put());
  assert(SUCCEEDED(hr));
  d3d11_frame_buffer->Release();

  // set up the default viewport to match the window
  {
    SDL_SysWMinfo system_info;
    SDL_VERSION(&system_info.version);
    SDL_GetWindowWMInfo(window, &system_info);
    HWND window_handle = system_info.info.win.window;

    RECT window_rect;
    GetClientRect(window_handle, &window_rect);
    D3D11_VIEWPORT viewport
    {
      0.0f,
      0.0f,
      (FLOAT)(window_rect.right - window_rect.left),
      (FLOAT)(window_rect.bottom - window_rect.top),
      0.0f,
      1.0f
    };
    device_context_->RSSetViewports(1, &viewport);
  }
}

Shader RenderDX11::CreateShader(LPCWSTR shader_path, int input_layout)
{
  HRESULT hr;
  ID3DBlob* error_buffer = nullptr;

  UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
  flags |= D3DCOMPILE_DEBUG;
#endif

  Shader shader;

  // Compile vertex shader
  hr = D3DCompileFromFile(
    shader_path,
    nullptr,
    D3D_COMPILE_STANDARD_FILE_INCLUDE,
    "vs_main",
    "vs_5_0",
    flags,
    0,
    shader.vertex_shader_buffer_.put(),
    &error_buffer
  );

  if (FAILED(hr)) {
    if (error_buffer) {
      std::clog << ((char*)error_buffer->GetBufferPointer());
      error_buffer->Release();
    }
    if (shader.vertex_shader_buffer_) { shader.vertex_shader_buffer_->Release(); }
    assert(!"Couldn't compile vertex shader");
  }

  // Compile pixel shader
  hr = D3DCompileFromFile(
    shader_path,
    nullptr,
    D3D_COMPILE_STANDARD_FILE_INCLUDE,
    "ps_main",
    "ps_5_0",
    flags,
    0,
    shader.pixel_shader_buffer_.put(),
    &error_buffer
  );

  if (FAILED(hr)) {
    if (error_buffer) {
      OutputDebugStringA((char*)error_buffer->GetBufferPointer());
      error_buffer->Release();
    }
    if (shader.pixel_shader_buffer_) { shader.pixel_shader_buffer_->Release(); }
    assert(!"Couldn't compile pixel shader");
  }

  hr = GetD3D11Device()->CreateVertexShader(
    shader.vertex_shader_buffer_->GetBufferPointer(),
    shader.vertex_shader_buffer_->GetBufferSize(),
    nullptr,
    shader.vertex_shader_.put()
  );
  assert(SUCCEEDED(hr));

  hr = GetD3D11Device()->CreatePixelShader(
    shader.pixel_shader_buffer_->GetBufferPointer(),
    shader.pixel_shader_buffer_->GetBufferSize(),
    nullptr,
    shader.pixel_shader_.put()
  );
  assert(SUCCEEDED(hr));

  D3D11_INPUT_ELEMENT_DESC input_element_descriptor[Shader::InputLayout_COUNT]{};
  int input_element_count = 0;
  for (int i = 0; i < Shader::InputLayout_COUNT; ++i)
  {
    if (input_layout & i)
    {
      ++input_element_count;
    }
  }
  assert(input_element_count > 0);

  int elements_filled = 0;
  if (input_layout & Shader::InputLayout_POS)
  {
    input_element_descriptor[elements_filled] =
    { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
      elements_filled ? D3D11_APPEND_ALIGNED_ELEMENT : 0,
      D3D11_INPUT_PER_VERTEX_DATA, 0 };
    ++elements_filled;
  }
  if (input_layout & Shader::InputLayout_COL)
  {
    input_element_descriptor[elements_filled] =
    { "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
      elements_filled ? D3D11_APPEND_ALIGNED_ELEMENT : 0,
      D3D11_INPUT_PER_VERTEX_DATA, 0 };
    ++elements_filled;
  }
  if (input_layout & Shader::InputLayout_NOR)
  {
    input_element_descriptor[elements_filled] =
    { "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
      elements_filled ? D3D11_APPEND_ALIGNED_ELEMENT : 0,
      D3D11_INPUT_PER_VERTEX_DATA, 0 };
    ++elements_filled;
  }
  if (input_layout & Shader::InputLayout_TEX)
  {
    input_element_descriptor[elements_filled] =
    { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
      elements_filled ? D3D11_APPEND_ALIGNED_ELEMENT : 0,
      D3D11_INPUT_PER_VERTEX_DATA, 0 };
    ++elements_filled;
  }

  hr = GetD3D11Device()->CreateInputLayout(
    input_element_descriptor,
    input_element_count,
    shader.vertex_shader_buffer_->GetBufferPointer(),
    shader.vertex_shader_buffer_->GetBufferSize(),
    shader.vertex_shader_input_layout_.put()
  );
  assert(SUCCEEDED(hr));

  return shader;
}

void RenderDX11::UseShader(Shader& shader)
{
  device_context_->VSSetShader(shader.vertex_shader_.get(), nullptr, 0);
  device_context_->PSSetShader(shader.pixel_shader_.get(), nullptr, 0);
}

ID3D11Device* RenderDX11::GetD3D11Device() const
{
  return device_.get();
}

ID3D11DeviceContext* RenderDX11::GetD3D11Context() const
{
  return device_context_.get();
}

void RenderDX11::ClearBuffers()
{
  DirectX::XMFLOAT4 clear_color{ clear_color_.x, clear_color_.y, clear_color_.z, 1.0 };
  device_context_->ClearRenderTargetView(render_target_view_.get(), &(clear_color.x));
  device_context_->ClearDepthStencilView(
    depth_stencil_view_.get(),
    D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
    1.0f,
    0
  );
}

}
