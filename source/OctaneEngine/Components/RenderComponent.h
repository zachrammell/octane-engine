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

struct RenderComponent
{
  Color color;
  MeshType mesh_type;
};

} // namespace Octane
