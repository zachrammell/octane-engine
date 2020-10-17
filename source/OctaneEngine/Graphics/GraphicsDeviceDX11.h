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
#include <OctaneEngine/Graphics/Mesh.h>
#include <OctaneEngine/Graphics/Shader.h>

namespace Octane
{
/*!
 * \brief The interface to DirectX 11, and manager of DX11 GPU resources.
 */
class GraphicsDeviceDX11
{
public:
  GraphicsDeviceDX11(SDL_Window* window);
  ~GraphicsDeviceDX11();

  void ClearScreen();
  void SetClearColor(Color c);

  void Present();
  void ResizeFramebuffer(SDL_Window* window);

  void SetFullscreen(bool is_fullscreen);

  [[nodiscard]] Shader CreateShader(LPCWSTR shader_path, int input_layout);
  void UseShader(Shader& shader);

  [[nodiscard]] MeshDX11 CreateMesh(Mesh const& mesh);
  void EmplaceMesh(MeshDX11* placement, Mesh const& mesh);
  void UseMesh(MeshDX11 const& mesh);
  void DrawMesh();

  ID3D11Device* GetD3D11Device() const;
  ID3D11DeviceContext* GetD3D11Context() const;

  class ShaderConstantBuffers
  {
  public:
    class PerObjectConstants
    {
    public:
      PerObjectConstants& SetWorldMatrix(DirectX::XMMATRIX world_matrix)
      {
        DirectX::XMStoreFloat4x4(&(raw_data_.World), world_matrix);
        return *this;
      }
      PerObjectConstants& SetWorldNormalMatrix(DirectX::XMMATRIX world_normal_matrix)
      {
        DirectX::XMStoreFloat4x4(&(raw_data_.NormalWorld), world_normal_matrix);
        return *this;
      }
      PerObjectConstants& SetColor(Color color)
      {
        raw_data_.Color = color.vec;
        return *this;
      }
      struct RawData
      {
        DirectX::XMFLOAT4X4 World;
        DirectX::XMFLOAT4X4 NormalWorld;
        DirectX::XMFLOAT3 Color;
        FLOAT padding0_;
      };

    private:
      friend class GraphicsDeviceDX11;
      RawData raw_data_;
    };

    class PerFrameConstants
    {
    public:
      PerFrameConstants& SetViewProjection(DirectX::XMMATRIX view_projection_matrix)
      {
        DirectX::XMStoreFloat4x4(&(raw_data_.ViewProjection), view_projection_matrix);
        return *this;
      }
      PerFrameConstants& SetCameraPosition(DirectX::XMFLOAT3 camera_position)
      {
        raw_data_.CameraPosition = camera_position;
        return *this;
      }
      PerFrameConstants& SetLightPosition(DirectX::XMFLOAT3 light_position)
      {
        raw_data_.LightPosition = light_position;
        return *this;
      }
      struct RawData
      {
        DirectX::XMFLOAT4X4 ViewProjection;
        DirectX::XMFLOAT3 CameraPosition;
        FLOAT padding0_;
        DirectX::XMFLOAT3 LightPosition;
        FLOAT padding1_;
      };

    private:
      friend class GraphicsDeviceDX11;
      RawData raw_data_;
    };

    PerObjectConstants& PerObject() { return per_object_; }
    PerFrameConstants& PerFrame() { return per_frame_; }

  private:
    PerObjectConstants per_object_ {};
    PerFrameConstants per_frame_ {};
  };

  ShaderConstantBuffers& ShaderConstants() { return shader_constants_; }

  void Upload(ShaderConstantBuffers::PerObjectConstants const& per_object_constants) const
  {
    GetD3D11Context()
      ->UpdateSubresource(constant_buffers_[0].get(), 0, nullptr, &(per_object_constants.raw_data_), 0, 0);
    {
      ID3D11Buffer* p_constant_buffers[1] = {constant_buffers_[0].get()};
      GetD3D11Context()->VSSetConstantBuffers(0, 1, p_constant_buffers);
      GetD3D11Context()->PSSetConstantBuffers(0, 1, p_constant_buffers);
    }
  }

  void Upload(ShaderConstantBuffers::PerFrameConstants const& per_frame_constants) const
  {
    GetD3D11Context()
      ->UpdateSubresource(constant_buffers_[1].get(), 0, nullptr, &(per_frame_constants.raw_data_), 0, 0);
    {
      ID3D11Buffer* p_constant_buffers[1] = {constant_buffers_[1].get()};
      GetD3D11Context()->VSSetConstantBuffers(1, 1, p_constant_buffers);
      GetD3D11Context()->PSSetConstantBuffers(1, 1, p_constant_buffers);
    }
  }

private:
  winrt::com_ptr<IDXGISwapChain> swap_chain_;
  winrt::com_ptr<ID3D11Device> device_;
  winrt::com_ptr<ID3D11DeviceContext> device_context_;
  winrt::com_ptr<ID3D11RenderTargetView> render_target_view_;
  winrt::com_ptr<ID3D11DepthStencilView> depth_stencil_view_;

  winrt::com_ptr<ID3D11DepthStencilState> depth_stencil_state_;
  winrt::com_ptr<ID3D11RasterizerState> rasterizer_state_;

  /*!
   * Shader programs' constant buffers. (Uniforms in OpenGL) \n
   * 0: per object constants                                 \n
   * 1: per frame constants
   */
  winrt::com_ptr<ID3D11Buffer> constant_buffers_[2];

  int w, h;
  Color clear_color_;
  MeshDX11 const* current_mesh_;
  ShaderConstantBuffers shader_constants_;

  void ClearBuffers();
};

} // namespace Octane
