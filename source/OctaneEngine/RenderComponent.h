#pragma once

#include <OctaneEngine/Graphics/Colors.h>

namespace Octane
{

enum class MeshType
{
  Cube,
  Sphere,
  Cube_Rounded,
  Bear,
  Duck,
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