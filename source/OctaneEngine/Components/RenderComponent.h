#pragma once

#include <OctaneEngine/Graphics/Colors.h>
#include <EASTL/string_view.h>

namespace Octane
{

enum class RenderType : int32_t
{
  Filled,
  Wireframe,
  Invisible,
  COUNT
};

enum class ShaderType : int32_t
{
  Phong,
  UI,
  PhongUI,
  COUNT
};

using Mesh_Key = eastl::string_view; //MeshSys mesh key type

struct RenderComponent
{
  Color color {};
  Mesh_Key mesh_type;
  RenderType render_type = RenderType::Filled;
  ShaderType shader_type = ShaderType::Phong;
};

} // namespace Octane
