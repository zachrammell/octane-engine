#pragma once

#include <OctaneEngine/Graphics/Mesh.h>

namespace Octane
{

class OBJParser
{
public:
  // it's a class for unimplemented buffering reasons
  OBJParser() = default;
  Mesh ParseOBJ(wchar_t const* filepath);
};

} // namespace Octane
