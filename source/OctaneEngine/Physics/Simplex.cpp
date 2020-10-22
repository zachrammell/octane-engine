#include <OctaneEngine/Physics/Simplex.h>

namespace Octane
{
SupportPoint& Simplex::operator[](size_t index)
{
  switch (index)
  {
  case 0: return simplex_vertex_d;
  case 1: return simplex_vertex_c;
  case 2: return simplex_vertex_b;
  case 3: return simplex_vertex_a;
  default: return simplex_vertex_a;
  }
}

SupportPoint const& Simplex::operator[](size_t index) const
{
  switch (index)
  {
  case 0: return simplex_vertex_d;
  case 1: return simplex_vertex_c;
  case 2: return simplex_vertex_b;
  case 3: return simplex_vertex_a;
  default: return simplex_vertex_a;
  }
}

bool Simplex::DoSimplexPoint(DirectX::XMVECTOR& dir)
{
  dir = DirectX::XMVectorNegate(DirectX::XMLoadFloat3(&simplex_vertex_a.global));
  simplex_vertex_b = simplex_vertex_a;
  count = 1;
  return false;
}

bool Simplex::DoSimplexLine(DirectX::XMVECTOR& dir)
{
  DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&simplex_vertex_b.global), DirectX::XMLoadFloat3(&simplex_vertex_a.global));
  DirectX::XMVECTOR ao = DirectX::XMVectorNegate(DirectX::XMLoadFloat3(&simplex_vertex_a.global));

  dir = Math::CrossProductTwice(ab, ao);
  simplex_vertex_c = simplex_vertex_b;
  simplex_vertex_b = simplex_vertex_a;
  count = 2;
  return false;
}

bool Simplex::DoSimplexTriangle(DirectX::XMVECTOR& dir)
{
  DirectX::XMVECTOR a_global = DirectX::XMLoadFloat3(&simplex_vertex_a.global);
  DirectX::XMVECTOR b_global = DirectX::XMLoadFloat3(&simplex_vertex_b.global);
  DirectX::XMVECTOR c_global = DirectX::XMLoadFloat3(&simplex_vertex_c.global);

  DirectX::XMVECTOR ao = DirectX::XMVectorNegate(a_global);
  DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(b_global, a_global);
  DirectX::XMVECTOR ac = DirectX::XMVectorSubtract(c_global, a_global);
  DirectX::XMVECTOR abc = DirectX::XMVector3Cross(ab, ac);
  DirectX::XMVECTOR ab_abc = DirectX::XMVector3Cross(ab, abc);
  if (Math::DotProductVector3(ab_abc, ao) > 0.0f)
  {
    simplex_vertex_c = simplex_vertex_a;
    dir = Math::CrossProductTwice(ab, ao);
    return false;
  }
  DirectX::XMVECTOR abc_ac = DirectX::XMVector3Cross(abc, ac);
  if (Math::DotProductVector3(abc_ac, ao) > 0.0f)
  {
    simplex_vertex_b = simplex_vertex_a;
    dir = Math::CrossProductTwice(ac, ao);
    return false;
  }
  if (Math::DotProductVector3(abc, ao) > 0.0f)
  {
    simplex_vertex_d = simplex_vertex_c;
    simplex_vertex_c = simplex_vertex_b;
    simplex_vertex_b = simplex_vertex_a;
    dir = abc;
  }
  else
  {
    simplex_vertex_d = simplex_vertex_b;
    simplex_vertex_b = simplex_vertex_a;
    dir = DirectX::XMVectorNegate(abc);
  }
  count = 3;
  return false;
}

bool Simplex::DoSimplexTetrahedron(DirectX::XMVECTOR& dir)
{
  DirectX::XMVECTOR a_global = DirectX::XMLoadFloat3(&simplex_vertex_a.global);
  DirectX::XMVECTOR b_global = DirectX::XMLoadFloat3(&simplex_vertex_b.global);
  DirectX::XMVECTOR c_global = DirectX::XMLoadFloat3(&simplex_vertex_c.global);
  DirectX::XMVECTOR d_global = DirectX::XMLoadFloat3(&simplex_vertex_d.global);

  DirectX::XMVECTOR ao = DirectX::XMVectorNegate(a_global);
  DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(b_global, a_global);
  DirectX::XMVECTOR ac = DirectX::XMVectorSubtract(c_global, a_global);
  DirectX::XMVECTOR ad = DirectX::XMVectorSubtract(d_global, a_global);
  dir = DirectX::XMVector3Cross(ab, ac);
  if (Math::DotProductVector3(ao, dir) > 0.0f)
  {
    //in front of abc
    simplex_vertex_d = simplex_vertex_c;
    simplex_vertex_c = simplex_vertex_b;
    simplex_vertex_b = simplex_vertex_a;
  }
  else
  {
    dir = DirectX::XMVector3Cross(ad, ab);
    if (Math::DotProductVector3(ao, dir) > 0.0f)
    {
      //in front of adb
      simplex_vertex_c = simplex_vertex_d;
      simplex_vertex_d = simplex_vertex_b;
      simplex_vertex_b = simplex_vertex_a;
    }
    else
    {
      dir = DirectX::XMVector3Cross(ac, ad);
      if (Math::DotProductVector3(ao, dir) > 0.0f)
      {
        //n front of acd
        simplex_vertex_b = simplex_vertex_a;
      }
      else
      {
        // origin is in the tetrahedron a-b-c-d
        count = 4;
        return true;
      }
    }
  }
  count = 3;
  return false;
}

bool Simplex::DoSimplex(DirectX::XMVECTOR& dir)
{
  if (count == 0)
  {
    return DoSimplexPoint(dir);
  }
  if (count == 1)
  {
    return DoSimplexLine(dir);
  }
  if (count == 2)
  {
    return DoSimplexTriangle(dir);
  }
  if (count == 3)
  {
    return DoSimplexTetrahedron(dir);
  }
  return false;
}

bool Simplex::IsValid()
{
  if (simplex_vertex_d.IsValid() == false)
  {
    count = 0;
    return false;
  }
  if (simplex_vertex_c.IsValid() == false)
  {
    count = 1;
    return false;
  }
  if (simplex_vertex_b.IsValid() == false)
  {
    count = 2;
    return false;
  }
  if (simplex_vertex_a.IsValid() == false)
  {
    count = 3;
    return false;
  }
  return true;
}

void Simplex::Set(const SupportPoint& a, const SupportPoint& b, const SupportPoint& c, const SupportPoint& d)
{
  count = 4;
  this->simplex_vertex_a = a;
  this->simplex_vertex_b = b;
  this->simplex_vertex_c = c;
  this->simplex_vertex_d = d;
}

void Simplex::Set(const SupportPoint& a, const SupportPoint& b, const SupportPoint& c)
{
  count = 3;
  this->simplex_vertex_a = a;
  this->simplex_vertex_b = b;
  this->simplex_vertex_c = c;
}

void Simplex::Set(const SupportPoint& a, const SupportPoint& b)
{
  count = 2;
  this->simplex_vertex_a = a;
  this->simplex_vertex_b = b;
}

void Simplex::Set(const SupportPoint& a)
{
  count = 1;
  this->simplex_vertex_a = a;
}
} // namespace Octane
