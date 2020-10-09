#pragma once

#include <OctaneEngine/Math.h>
#include <OctaneEngine/Physics/Primitive.h>
#include <OctaneEngine/Physics/Simplex.h>
#include <OctaneEngine/Physics/SupportPoint.h>
#include <OctaneEngine/Physics/Polytope.h>
#include <OctaneEngine/Physics/ContactConstraints.h>


namespace Octane
{

class NarrowPhase
{
public:
  NarrowPhase() = default;
  ~NarrowPhase() = default;

  SupportPoint GenerateCSOSupport(Primitive* a, Primitive* b, const DirectX::XMVECTOR& direction);
  bool GJKCollisionDetection(Primitive* a, Primitive* b, Simplex& simplex);
  bool EPAContactGeneration(Primitive* a, Primitive* b, Polytope& polytope, ContactPoint& result);

private:
  size_t m_gjk_exit_iteration = 100;
  size_t m_epa_exit_iteration = 100;

  DirectX::XMVECTOR m_search_directions[6];
  DirectX::XMVECTOR m_basis[3];
};

} // namespace Octane
