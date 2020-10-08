#pragma once

#include <OctaneEngine/Graphics/Colors.h>

namespace Octane
{

enum class MeshType
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