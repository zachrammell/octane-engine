#pragma once

#include <OctaneEngine/Graphics/Colors.h>

namespace Octane
{

enum MeshType
{
  Cube,
  Sphere,
};

struct RenderComponent
{
  Color color;
  MeshType mesh_type;
};
} // namespace Octane