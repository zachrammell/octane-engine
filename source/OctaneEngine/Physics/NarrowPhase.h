#pragma once

#include <OctaneEngine/Math.h>
#include <OctaneEngine/Physics/Primitive.h>
#include <OctaneEngine/Physics/SupportPoint.h>
#include <OctaneEngine/Physics/Simplex.h>

namespace Octane
{
class NarrowPhase
{
public:
  NarrowPhase() = default;
  ~NarrowPhase() = default;

   SupportPoint GenerateCSOSupport(Primitive* a, Primitive* b, const DirectX::XMVECTOR& direction);
  bool GJKCollisionDetection(Primitive* a, Primitive* b, Simplex& simplex);


private:
  size_t m_gjk_exit_iteration = 100;

  DirectX::XMVECTOR m_search_directions[6];
  DirectX::XMVECTOR m_basis[3];
};
}
