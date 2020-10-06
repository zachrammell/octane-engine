#pragma once
#include <OctaneEngine/Physics/Primitive.h>

namespace Octane
{
class PrimitivePair
{
public:
  PrimitivePair(Primitive* a, Primitive* b) : a(a), b(b) {}

  ~PrimitivePair() {}

  void Swap()
  {
    auto temp = a;
    a = b;
    b = temp;
  }

public:
  Primitive* a = nullptr;
  Primitive* b = nullptr;
};
} // namespace Octane
