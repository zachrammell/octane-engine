#include <OctaneEngine/Physics/NarrowPhase.h>
#include <OctaneEngine/Physics/PrimitivePair.h>
#include <OctaneEngine/Physics/RigidBody.h>

namespace Octane
{
NarrowPhase::NarrowPhase()
{
  basis_[0] = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
  basis_[1] = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
  basis_[2] = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

  search_dir_[0] = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
  search_dir_[1] = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
  search_dir_[2] = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
  search_dir_[3] = DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
  search_dir_[4] = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
  search_dir_[5] = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
}

void NarrowPhase::GenerateContact(
  eastl::vector<PrimitivePair> potential_pairs_,
  eastl::hash_map<size_t, ContactManifold>* manifold_table)
{
  //filter prev manifolds
  for (auto& pair : potential_pairs_)
  {
    Simplex simplex;
    if (GJKCollisionDetection(pair.a, pair.b, simplex) == true)
    {
      //collider pair have a collision do epa and create collision.
      if (simplex.IsValid() == false)
      {
        if (simplex.count == 0)
        {
          continue;
        }
      }
      Polytope polytope = Polytope(simplex);
      ContactPoint new_contact_data;
      if (EPAContactGeneration(pair.a, pair.b, polytope, new_contact_data) == true)
      {
        //send a event about start and persist.
        size_t key = reinterpret_cast<size_t>(pair.a) + reinterpret_cast<size_t>(pair.b);
        auto found = manifold_table->find(key);

        if (found == manifold_table->end())
        {
          found = manifold_table->emplace(key, ContactManifold(pair.a, pair.b)).first;
        }

        ContactManifold& manifold = found->second;

        manifold.UpdateCurrentManifold(new_contact_data);
        manifold.CutDownManifold();
        manifold.is_collide = true;
      }
      else
      {
        //generate invalid contact do not copy data.
        //send a event invalid.
      }
    }
    else
    {
      //if gjk result false, they are not colliding each other.
      //send a event about none and end.
      size_t key = reinterpret_cast<size_t>(pair.a) + reinterpret_cast<size_t>(pair.b);
      auto found = manifold_table->find(key);

      if (found != manifold_table->end())
      {
        found->second.ClearContacts();
        found->second.is_collide = false;
      }
    }
  }
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

  auto a_global = DirectX::XMLoadFloat3(&simplex.simplex_vertex_a.global);

  for (size_t i = 0; i < gjk_exit_count_; ++i)
  {
    if (Math::IsValid(direction) == false)
    {
      //if director divide by 0 or infinite, nan
      return false;
    }
    simplex.simplex_vertex_a = GenerateCSOSupport(a, b, direction);
    if (Math::DotProductVector3(a_global, direction) < 0.0f)
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
  for (size_t i = 0; i < epa_exit_count_; ++i)
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
    float distance = Math::DotProductVector3(DirectX::XMLoadFloat3(&support_point.global), closest_face.normal);
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

  auto a_a_global = DirectX::XMLoadFloat3(&polytope.vertices[closest_face.a].local_a);
  auto b_a_global = DirectX::XMLoadFloat3(&polytope.vertices[closest_face.b].local_a);
  auto c_a_global = DirectX::XMLoadFloat3(&polytope.vertices[closest_face.c].local_a);

  auto a_b_global = DirectX::XMLoadFloat3(&polytope.vertices[closest_face.a].local_b);
  auto b_b_global = DirectX::XMLoadFloat3(&polytope.vertices[closest_face.b].local_b);
  auto c_b_global = DirectX::XMLoadFloat3(&polytope.vertices[closest_face.c].local_b);


  result.local_position_a = DirectX::XMVectorAdd(
    DirectX::XMVectorAdd(
      DirectX::XMVectorScale(a_a_global, u),
      DirectX::XMVectorScale(b_a_global, v)),
    DirectX::XMVectorScale(c_a_global, w));
  result.local_position_b = DirectX::XMVectorAdd(
    DirectX::XMVectorAdd(
      DirectX::XMVectorScale(a_b_global, u),
      DirectX::XMVectorScale(b_b_global, v)),
    DirectX::XMVectorScale(c_b_global, w));
  result.global_position_a = a->GetRigidBody()->LocalToWorldPoint(result.local_position_a);
  result.global_position_b = b->GetRigidBody()->LocalToWorldPoint(result.local_position_b);
  result.is_valid = true;
  result.normal = DirectX::XMVector3Normalize(closest_face.normal);
  result.depth = closest_face.distance;
  return true;
}
} // namespace Octane
