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
  PaperPlane,
  Shuriken,
  PaperStack,
  TestFBX,

  COUNT
};

enum class RenderType : int32_t
{
  Filled,
  Wireframe,

  COUNT
};

struct RenderComponent
{
  Color color {};
  MeshType mesh_type = MeshType::INVALID;
  RenderType render_type = RenderType::Filled;
};

} // namespace Octane
