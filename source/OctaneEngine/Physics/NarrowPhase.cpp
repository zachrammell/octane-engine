#include <OctaneEngine/Physics/NarrowPhase.h>
#include <OctaneEngine/Physics/PrimitivePair.h>
#include <OctaneEngine/Physics/RigidBody.h>

namespace Octane
{
NarrowPhase::NarrowPhase()
{
  m_basis[0] = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
  m_basis[1] = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
  m_basis[2] = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

  m_search_directions[0] = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
  m_search_directions[1] = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
  m_search_directions[2] = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
  m_search_directions[3] = DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
  m_search_directions[4] = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
  m_search_directions[5] = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
}

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

bool NarrowPhase::EPAContactGeneration(Primitive* a, Primitive* b, Polytope& polytope, ContactPoint& result)
{
  PolytopeFace closest_face = polytope.PickClosestFace();
  PolytopeFace prev_face = closest_face;
  for (size_t i = 0; i < m_epa_exit_iteration; ++i)
  {
    if (polytope.faces.empty())
    {
      result.is_valid = false;
      return false;
    }
    closest_face = polytope.PickClosestFace();
    SupportPoint support_point = GenerateCSOSupport(a, b, closest_face.normal);
    if (support_point.IsValid() == false)
    {
      closest_face = prev_face;
      break;
    }
    float distance = Math::DotProductVector3(support_point.global, closest_face.normal);
    if (distance - closest_face.distance < Math::EPSILON)
    {
      break;
    }
    polytope.Push(support_point);
    polytope.Expand(support_point);
    prev_face = closest_face;
  }
  float u, v, w;
  closest_face
    .BarycentricCoordinates(DirectX::XMVectorScale(closest_face.normal, closest_face.distance), u, v, w, &polytope);
  if (Math::IsValid(u) == false || Math::IsValid(v) == false || Math::IsValid(w) == false)
  {
    result.is_valid = false;
    return false;
  }
  if (fabsf(u) > 1.0f || fabsf(v) > 1.0f || fabsf(w) > 1.0f)
  {
    result.is_valid = false;
    return false;
  }
  result.collider_a = a;
  result.collider_b = b;
  result.local_position_a = DirectX::XMVectorAdd(
    DirectX::XMVectorAdd(
      DirectX::XMVectorScale(polytope.vertices[closest_face.a].local_a, u),
      DirectX::XMVectorScale(polytope.vertices[closest_face.b].local_a, v)),
    DirectX::XMVectorScale(polytope.vertices[closest_face.c].local_a, w));
  result.local_position_b = DirectX::XMVectorAdd(
    DirectX::XMVectorAdd(
      DirectX::XMVectorScale(polytope.vertices[closest_face.a].local_b, u),
      DirectX::XMVectorScale(polytope.vertices[closest_face.b].local_b, v)),
    DirectX::XMVectorScale(polytope.vertices[closest_face.c].local_b, w));
  result.global_position_a = a->GetRigidBody()->LocalToWorldPoint(result.local_position_a);
  result.global_position_b = b->GetRigidBody()->LocalToWorldPoint(result.local_position_b);
  result.is_valid = true;
  result.normal = DirectX::XMVector3Normalize(closest_face.normal);
  result.depth = closest_face.distance;
  return true;
}
} // namespace Octane
