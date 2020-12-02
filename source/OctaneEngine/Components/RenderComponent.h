#pragma once

#include <OctaneEngine/Graphics/Colors.h>
#include <OctaneEngine/Graphics/Material.h>
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

typedef eastl::string_view Mesh_Key; //MeshSys mesh key type

struct RenderComponent
{
  Material material;
  Mesh_Key mesh_type;
  RenderType render_type = RenderType::Filled;
  ShaderType shader_type = ShaderType::Phong;

};

} // namespace Octane
