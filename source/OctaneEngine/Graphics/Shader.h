#pragma once
#include <d3d11.h>
#include <winrt/base.h>

namespace Octane
{

class Shader
{
public:
  // TODO: replace InputLayout system with parsing the shader file and handling
  // it automatically
  enum InputLayout
  {
    InputLayout_POS = 1 << 0,
    InputLayout_COL = 1 << 1,
    InputLayout_NOR = 1 << 2,
    InputLayout_TEX = 1 << 3,
    // bookkeeping
    InputLayout_COUNT = 4,
  };

  friend class RenderDX11;
  ID3D11InputLayout* GetInputLayout() { return vertex_shader_input_layout_.get(); }

private:
  winrt::com_ptr<ID3D11VertexShader> vertex_shader_;
  winrt::com_ptr<ID3D11PixelShader> pixel_shader_;
  winrt::com_ptr<ID3DBlob> vertex_shader_buffer_;
  winrt::com_ptr<ID3DBlob> pixel_shader_buffer_;
  winrt::com_ptr<ID3D11InputLayout> vertex_shader_input_layout_;
};

} // namespace Octane
