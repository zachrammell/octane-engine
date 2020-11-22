#pragma once

#include <OctaneEngine/Graphics/Colors.h>

namespace Octane
{

enum class MeshType : int32_t
{
  INVALID = -1,
  Cube,
  Sphere,
  Cube_Rounded,
  Bear,
  Duck,
  Bunny,
  Crossbow,
  Slingshot,
  Shuriken,
  PaperPlane,
  TestFBX,
  PaperStack,
  Quad,
  Sword,
  Sniper1,
  Semiauto1,

  COUNT
};

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

  COUNT
};

struct RenderComponent
{
  Color color {};
  MeshType mesh_type = MeshType::INVALID;
  RenderType render_type = RenderType::Filled;
  ShaderType shader_type = ShaderType::Phong;
};

} // namespace Octane
