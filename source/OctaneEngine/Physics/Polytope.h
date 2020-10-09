#pragma once

#include <OctaneEngine/Math.h>
#include <OctaneEngine/Physics/SupportPoint.h>
#include <EASTL/vector.h>

namespace Octane
{
class Simplex;

class PolytopeEdge
{
public:
  PolytopeEdge();
  PolytopeEdge(size_t a, size_t b);
  PolytopeEdge(const SupportPoint& point_a, const SupportPoint& point_b);
  ~PolytopeEdge();

public:
  size_t a, b;
};

class Polytope;

class PolytopeFace
{
public:
  PolytopeFace();
  PolytopeFace(size_t a, size_t b, size_t c, const DirectX::XMVECTOR& normal, float distance);
  PolytopeFace(size_t a, size_t b, size_t c, const Simplex& simplex);
  PolytopeFace(size_t a, size_t b, size_t c, const Polytope& polytope);
  PolytopeFace(const SupportPoint& point_a, const SupportPoint& point_b, const SupportPoint& point_c);
  ~PolytopeFace();

  size_t& operator[](size_t index);
  size_t operator[](size_t index) const;

  PolytopeFace& operator=(const PolytopeFace& rhs);

  bool operator==(const PolytopeFace& rhs) const;

  size_t& Index(size_t index);
  size_t Index(size_t index) const;

  float TestFace(const DirectX::XMVECTOR& point) const;
  void BarycentricCoordinates(const DirectX::XMVECTOR& point, float& u, float& v, float& w, Polytope* polytope) const;

public:
  size_t a{}, b{}, c{};
  DirectX::XMVECTOR normal{};
  float distance{};
};

class Polytope
{
public:
  explicit Polytope(const Simplex& simplex);
  ~Polytope() = default;

  PolytopeFace PickClosestFace();
  float PointFaceTest(const DirectX::XMVECTOR& point, const DirectX::XMVECTOR& normal, const DirectX::XMVECTOR& v0);
  void Push(SupportPoint& vertex);
  void PushFromSimplex(const SupportPoint& vertex);
  void Expand(const SupportPoint& vertex);
  bool IsFaceSeen(const PolytopeFace& face, const SupportPoint& vertex);
  void AddEdge(size_t a, size_t b);

private:
  friend class NarrowPhase;
  friend class PolytopeFace;

private:
  eastl::vector<SupportPoint> vertices;
  eastl::vector<PolytopeEdge> edges;
  eastl::vector<PolytopeFace> faces;
};
} // namespace Octane
