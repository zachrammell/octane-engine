#pragma once

#include <OctaneEngine/Math.h>

namespace Octane
{
class Primitive;

class BoundingAABB
{
public:
  explicit BoundingAABB(Primitive* collider = nullptr);
  ~BoundingAABB();

private:
};


}
