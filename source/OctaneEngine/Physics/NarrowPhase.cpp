#include <OctaneEngine/Physics/NarrowPhase.h>
#include <OctaneEngine/Physics/RigidBody.h>
#include <OctaneEngine/Physics/PrimitivePair.h>

namespace Octane
{
SupportPoint NarrowPhase::GenerateCSOSupport(Primitive* a, Primitive* b, const DirectX::XMVECTOR& direction)
{
  RigidBody* body_a = a->GetRigidBody();
  RigidBody* body_b = b->GetRigidBody();
  DirectX::XMVECTOR local_a = a->Support(DirectX::XMVector3Normalize(body_a->WorldToLocalVector(direction)));
  DirectX::XMVECTOR local_b
    = b->Support(DirectX::XMVector3Normalize(body_b->WorldToLocalVector(DirectX::XMVectorNegate(direction))));
  DirectX::XMVECTOR support_a = body_a->LocalToWorldPoint(local_a);
  DirectX::XMVECTOR support_b = body_b->LocalToWorldPoint(local_b);
  return SupportPoint(DirectX::XMVectorSubtract(support_a, support_b), local_a, local_b);
}

bool NarrowPhase::GJKCollisionDetection(Primitive* a, Primitive* b, Simplex& simplex)
{
  DirectX::XMVECTOR direction = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
  for (size_t i = 0; i < m_gjk_exit_iteration; ++i)
  {
    if (Math::IsValid(direction) == false)
    {
      //if director divide by 0 or infinite, nan
      return false;
    }
    simplex.simplex_vertex_a = GenerateCSOSupport(a, b, direction);
    if (Math::DotProductVector3(simplex.simplex_vertex_a.global, direction) < 0.0f)
    {
      return false;
    }
    if (simplex.DoSimplex(direction) == true)
    {
      return true;
    }
  }
  return false;
}
} // namespace Octane
