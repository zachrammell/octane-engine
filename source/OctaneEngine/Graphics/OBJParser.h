#pragma once

#include <DirectXMath.h>

namespace Octane
{

struct Mesh
{
  struct Vertex
  {
    DirectX::XMFLOAT3 position = {};
    DirectX::XMFLOAT3 normal   = {};
  };
  using Index = uint32_t;

  Vertex* vertex_buffer;
  size_t vertex_count;
  Index* index_buffer;
  size_t index_count;
};

class OBJParser
{
public:
  // it's a class for unimplemented buffering reasons
  OBJParser() = default;
  Mesh ParseOBJ(wchar_t const* filepath);
};

} // namespace Octane
