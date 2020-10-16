#pragma once
#include <d3d11.h>
#include <winrt/base.h>

namespace Octane
{

/*!
 * \brief A DirectX 11 GPU resource holder for a shader program. \n
 * Currently supports vertex and pixel shader stages. \n
 * TODO: add support for geometry shader and other shader stages.
 */
class Shader
{
public:
  /* TODO: replace InputLayout system with parsing the shader file and handling it automatically */
  enum InputLayout
  {
    InputLayout_POS = 1 << 0,
    InputLayout_COL = 1 << 1,
    InputLayout_NOR = 1 << 2,
    InputLayout_TEX = 1 << 3,
    // bookkeeping
    InputLayout_COUNT = 4,
  };

  ID3D11InputLayout* GetInputLayout() { return vertex_shader_input_layout_.get(); }

private:
  friend class GraphicsDeviceDX11;
  winrt::com_ptr<ID3D11VertexShader> vertex_shader_;
  winrt::com_ptr<ID3D11PixelShader> pixel_shader_;
  winrt::com_ptr<ID3DBlob> vertex_shader_buffer_;
  winrt::com_ptr<ID3DBlob> pixel_shader_buffer_;
  winrt::com_ptr<ID3D11InputLayout> vertex_shader_input_layout_;
};

} // namespace Octane
