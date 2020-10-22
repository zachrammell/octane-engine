#pragma once

#include <OctaneEngine/Math.h>
#include <OctaneEngine/Physics/SupportPoint.h>

namespace Octane
{
class Simplex
{
public:
  Simplex() = default;
  ~Simplex() = default;
  SupportPoint& operator[](size_t index);
  SupportPoint const& operator[](size_t index) const;

  bool DoSimplexPoint(DirectX::XMVECTOR& dir);
  bool DoSimplexLine(DirectX::XMVECTOR& dir);
  bool DoSimplexTriangle(DirectX::XMVECTOR& dir);
  bool DoSimplexTetrahedron(DirectX::XMVECTOR& dir);

  bool DoSimplex(DirectX::XMVECTOR& dir);
  bool IsValid();

  void Set(const SupportPoint& a, const SupportPoint& b, const SupportPoint& c, const SupportPoint& d);
  void Set(const SupportPoint& a, const SupportPoint& b, const SupportPoint& c);
  void Set(const SupportPoint& a, const SupportPoint& b);
  void Set(const SupportPoint& a);

private:
  friend class NarrowPhase;
  friend class Polytope;

private:
  SupportPoint simplex_vertex_a; //point
  SupportPoint simplex_vertex_b; //line_segment
  SupportPoint simplex_vertex_c; //triangle
  SupportPoint simplex_vertex_d; //tetrahedron

  int count = 0;
};
} // namespace Octane
