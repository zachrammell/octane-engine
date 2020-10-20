#pragma once

#include <OctaneEngine/Math.h>
#include <OctaneEngine/Physics/ContactConstraints.h>
#include <OctaneEngine/Physics/Polytope.h>
#include <OctaneEngine/Physics/Primitive.h>
#include <OctaneEngine/Physics/PrimitivePair.h>
#include <OctaneEngine/Physics/Simplex.h>
#include <OctaneEngine/Physics/SupportPoint.h>
#include <OctaneEngine/Physics/BoundingAABB.h>

#include <EASTL/hash_map.h>

namespace Octane
{
class NarrowPhase
{
public:
  NarrowPhase();
  ~NarrowPhase() = default;

  void GenerateContact(
    eastl::vector<PrimitivePair> potential_pairs_,
    eastl::hash_map<size_t, ContactManifold>* manifold_table);

  SupportPoint GenerateCSOSupport(Primitive* a, Primitive* b, const DirectX::XMVECTOR& direction);
  bool GJKCollisionDetection(Primitive* a, Primitive* b, Simplex& simplex);
  bool EPAContactGeneration(Primitive* a, Primitive* b, Polytope& polytope, ContactPoint& result);

private:
  size_t gjk_exit_count_ = 100;
  size_t epa_exit_count_ = 100;

  DirectX::XMVECTOR search_dir_[6];
  DirectX::XMVECTOR basis_[3];
};
} // namespace Octane
