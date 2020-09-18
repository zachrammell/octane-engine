#pragma once

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define NOMINMAX
#define UNICODE
// Windows.h needs to be included before d3d11.h, don't remove
#include <Windows.h>
#include <d3d11.h>
#include <winrt/base.h>

#include <SDL.h>

#include <OctaneEngine/Graphics/Colors.h>
#include <OctaneEngine/Graphics/Shader.h>

namespace Octane
{

class RenderDX11
{
public:
  RenderDX11(SDL_Window* window);
  ~RenderDX11();

  void DrawScene();
  void SetClearColor(Color c);

  void Present();
  void ResizeFramebuffer(SDL_Window* window);

  [[nodiscard]] Shader CreateShader(LPCWSTR shader_path, int input_layout);
  void UseShader(Shader& shader);

  ID3D11Device* GetD3D11Device() const;
  ID3D11DeviceContext* GetD3D11Context() const;

private:
  winrt::com_ptr<IDXGISwapChain> swap_chain_;
  winrt::com_ptr<ID3D11Device> device_;
  winrt::com_ptr<ID3D11DeviceContext> device_context_;
  winrt::com_ptr<ID3D11RenderTargetView> render_target_view_;
  winrt::com_ptr<ID3D11DepthStencilView> depth_stencil_view_;

  winrt::com_ptr<ID3D11DepthStencilState> depth_stencil_state_;
  winrt::com_ptr<ID3D11RasterizerState> rasterizer_state_;

  Color clear_color_;

  void ClearBuffers();
};

} // namespace Octane
