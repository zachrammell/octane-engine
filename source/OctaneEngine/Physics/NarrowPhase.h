#pragma once

#include <OctaneEngine/Math.h>
#include <OctaneEngine/Physics/ContactConstraints.h>
#include <OctaneEngine/Physics/Polytope.h>
#include <OctaneEngine/Physics/Primitive.h>
#include <OctaneEngine/Physics/PrimitivePair.h>
#include <OctaneEngine/Physics/Simplex.h>
#include <OctaneEngine/Physics/SupportPoint.h>
#include <OctaneEngine/Physics/BoundingAABB.h>
#include <OctaneEngine/TransformComponent.h>

#include <EASTL/hash_map.h>

namespace Octane
{
enum class eCollisionState
{
  None,
  Start,
  Persist,
  End,
  Invalid
};

class NarrowPhase
{
public:
  NarrowPhase();
  ~NarrowPhase() = default;

  void GenerateContact(
    eastl::vector<PrimitivePair> potential_pairs_,
    eastl::hash_map<size_t, ContactManifold>* manifold_table,
    eastl::hash_map<size_t, eCollisionState>* collision_table);

  SupportPoint GenerateCSOSupport(Primitive* a, Primitive* b, const DirectX::XMVECTOR& direction);
  bool GJKCollisionDetection(Primitive* a, Primitive* b, Simplex& simplex);
  bool EPAContactGeneration(Primitive* a, Primitive* b, Polytope& polytope, ContactPoint& result);

private:
  SupportPoint GenerateCSOSupport(
    const TransformComponent& transform_a,
    Primitive* primitive_a,
    const TransformComponent& transform_b,
    Primitive* primitive_b, const DirectX::XMVECTOR& direction);
  bool GJKCollisionDetection(
    const TransformComponent& transform_a,
    Primitive* primitive_a,
    const TransformComponent& transform_b,
    Primitive* primitive_b,
    size_t exit_count,
    Simplex& simplex);

private:
  friend class PhysicsSys;
  size_t gjk_exit_count_ = 100;
  size_t epa_exit_count_ = 100;

  DirectX::XMVECTOR search_dir_[6];
  DirectX::XMVECTOR basis_[3];
};
} // namespace Octane
