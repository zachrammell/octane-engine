#include <OctaneEngine/Physics/Polytope.h>
#include <OctaneEngine/Physics/Simplex.h>

namespace Octane
{
PolytopeEdge::PolytopeEdge()
{
  a = std::numeric_limits<size_t>::max();
  b = std::numeric_limits<size_t>::max();
}

PolytopeEdge::PolytopeEdge(size_t a, size_t b) : a(a), b(b) {}

PolytopeEdge::PolytopeEdge(const SupportPoint& point_a, const SupportPoint& point_b)
{
  a = point_a.index;
  b = point_b.index;
}

PolytopeEdge::~PolytopeEdge() {}

PolytopeFace::PolytopeFace()
{
  a = std::numeric_limits<size_t>::max();
  b = std::numeric_limits<size_t>::max();
  c = std::numeric_limits<size_t>::max();
}

PolytopeFace::PolytopeFace(size_t a, size_t b, size_t c, const DirectX::XMVECTOR& normal, float distance)
  : a(a),
    b(b),
    c(c),
    normal(normal),
    distance(distance)
{
}

PolytopeFace::PolytopeFace(size_t a, size_t b, size_t c, const Simplex& simplex) : a(a), b(b), c(c)
{
  auto a_global = DirectX::XMLoadFloat3(&simplex[a].global);
  auto b_global = DirectX::XMLoadFloat3(&simplex[b].global);
  auto c_global = DirectX::XMLoadFloat3(&simplex[c].global);

  normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(
    DirectX::XMVectorSubtract(b_global, a_global),
    DirectX::XMVectorSubtract(c_global, a_global)));

  distance = Math::DotProductVector3(normal, a_global);
}

PolytopeFace::PolytopeFace(size_t a, size_t b, size_t c, const Polytope& polytope) : a(a), b(b), c(c)
{
  auto a_global = DirectX::XMLoadFloat3(&polytope.vertices[a].global);
  auto b_global = DirectX::XMLoadFloat3(&polytope.vertices[b].global);
  auto c_global = DirectX::XMLoadFloat3(&polytope.vertices[c].global);

  normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(
    DirectX::XMVectorSubtract(b_global, a_global),
    DirectX::XMVectorSubtract(c_global, a_global)));

  distance = Math::DotProductVector3(normal, a_global);
  ;
}

PolytopeFace::PolytopeFace(const SupportPoint& point_a, const SupportPoint& point_b, const SupportPoint& point_c)
{
  auto a_global = DirectX::XMLoadFloat3(&point_a.global);
  auto b_global = DirectX::XMLoadFloat3(&point_b.global);
  auto c_global = DirectX::XMLoadFloat3(&point_c.global);

  a = point_a.index;
  b = point_b.index;
  c = point_c.index;
  normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(
    DirectX::XMVectorSubtract(b_global, a_global),
    DirectX::XMVectorSubtract(c_global, a_global)));

  distance = Math::DotProductVector3(normal, a_global);
}

PolytopeFace::~PolytopeFace() {}

size_t& PolytopeFace::operator[](size_t index)
{
  size_t i = index % 3;
  switch (i)
  {
  case 0: return a;
  case 1: return b;
  case 2: return c;
  default: return a;
  }
}

size_t PolytopeFace::operator[](size_t index) const
{
  size_t i = index % 3;
  switch (i)
  {
  case 0: return a;
  case 1: return b;
  case 2: return c;
  default: return a;
  }
}

PolytopeFace& PolytopeFace::operator=(const PolytopeFace& rhs)
{
  if (this != &rhs)
  {
    this->a = rhs.a;
    this->b = rhs.b;
    this->c = rhs.c;
    this->normal = rhs.normal;
    this->distance = rhs.distance;
  }
  return *this;
}

bool PolytopeFace::operator==(const PolytopeFace& rhs) const
{
  return (this->a == rhs.a && this->b == rhs.b && this->c == rhs.c);
}

size_t& PolytopeFace::Index(size_t index)
{
  size_t i = index % 3;
  switch (i)
  {
  case 0: return a;
  case 1: return b;
  case 2: return c;
  default: return a;
  }
}

size_t PolytopeFace::Index(size_t index) const
{
  size_t i = index % 3;
  switch (i)
  {
  case 0: return a;
  case 1: return b;
  case 2: return c;
  default: return a;
  }
}

float PolytopeFace::TestFace(const DirectX::XMVECTOR& point) const
{
  return Math::DotProductVector3(normal, point);
}

void PolytopeFace::BarycentricCoordinates(
  const DirectX::XMVECTOR& point,
  float& u,
  float& v,
  float& w,
  Polytope* polytope) const
{
  auto a_global = DirectX::XMLoadFloat3(&polytope->vertices[a].global);
  auto b_global = DirectX::XMLoadFloat3(&polytope->vertices[b].global);
  auto c_global = DirectX::XMLoadFloat3(&polytope->vertices[c].global);

  DirectX::XMVECTOR v0 = DirectX::XMVectorSubtract(b_global, a_global);
  DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(c_global, a_global);
  DirectX::XMVECTOR v2 = DirectX::XMVectorSubtract(point, a_global);
  float d00 = Math::DotProductVector3(v0, v0);
  float d01 = Math::DotProductVector3(v0, v1);
  float d11 = Math::DotProductVector3(v1, v1);
  float d20 = Math::DotProductVector3(v2, v0);
  float d21 = Math::DotProductVector3(v2, v1);
  float denominator = d00 * d11 - d01 * d01;
  v = (d11 * d20 - d01 * d21) / denominator;
  w = (d00 * d21 - d01 * d20) / denominator;
  u = 1.0f - v - w;
}

Polytope::Polytope(const Simplex& simplex)
{
  PushFromSimplex(simplex.simplex_vertex_d);
  PushFromSimplex(simplex.simplex_vertex_c);
  PushFromSimplex(simplex.simplex_vertex_b);
  PushFromSimplex(simplex.simplex_vertex_a);
  faces.emplace_back(this->vertices[3], this->vertices[2], this->vertices[1]);
  faces.emplace_back(this->vertices[3], this->vertices[1], this->vertices[0]);
  faces.emplace_back(this->vertices[3], this->vertices[0], this->vertices[2]);
  faces.emplace_back(this->vertices[2], this->vertices[0], this->vertices[1]);
}

PolytopeFace Polytope::PickClosestFace()
{
  //not initialized polytope return error polytope.
  if (faces.empty() == true)
  {
    return PolytopeFace(
      std::numeric_limits<size_t>::max(),
      std::numeric_limits<size_t>::max(),
      std::numeric_limits<size_t>::max(),
      DirectX::XMVECTOR(),
      0.0f);
  }

  auto closest_face_iterator = faces.end();
  float closest_face_distance = Math::REAL_POSITIVE_MAX;

  for (auto it = faces.begin(); it != faces.end(); ++it)
  {
    if ((*it).distance < closest_face_distance)
    {
      closest_face_distance = (*it).distance;
      closest_face_iterator = it;
    }
  }
  if (closest_face_iterator == faces.end())
  {
    return PolytopeFace(
      std::numeric_limits<size_t>::max(),
      std::numeric_limits<size_t>::max(),
      std::numeric_limits<size_t>::max(),
      DirectX::XMVECTOR(),
      0.0f);
  }
  return *closest_face_iterator;
}

float Polytope::PointFaceTest(
  const DirectX::XMVECTOR& point,
  const DirectX::XMVECTOR& normal,
  const DirectX::XMVECTOR& v0)
{
  DirectX::XMVECTOR unit = DirectX::XMVector3Normalize(normal);
  float d = Math::DotProductVector3(DirectX::XMVectorNegate(unit), v0);
  return Math::DotProductVector3(normal, point) + d;
}

void Polytope::Push(SupportPoint& vertex)
{
  vertex.index = this->vertices.size();
  vertices.emplace_back(vertex);
}

void Polytope::PushFromSimplex(const SupportPoint& vertex)
{
  auto index = this->vertices.size();
  vertices.emplace_back(vertex.global, vertex.local_a, vertex.local_b, index);
}

void Polytope::Expand(const SupportPoint& vertex)
{
  this->edges.clear();
  for (auto it = faces.begin(); it != faces.end();)
  {
    if (IsFaceSeen(*it, vertex))
    {
      //remove face in faces;
      this->AddEdge(it->a, it->b);
      this->AddEdge(it->b, it->c);
      this->AddEdge(it->c, it->a);
      it = faces.erase(it);
    }
    else
    {
      ++it;
    }
  }
  //add a new face cover up hole on polytope.
  for (auto it = edges.begin(); it != edges.end(); ++it)
  {
    //add a new face
    faces.emplace_back(this->vertices[it->a], this->vertices[it->b], vertex);
  }
}

bool Polytope::IsFaceSeen(const PolytopeFace& face, const SupportPoint& vertex)
{
  auto a_global = DirectX::XMLoadFloat3(&vertices[face.a].global);
  auto b_global = DirectX::XMLoadFloat3(&vertices[face.b].global);
  auto c_global = DirectX::XMLoadFloat3(&vertices[face.c].global);
  auto vertex_global = DirectX::XMLoadFloat3(&vertex.global);

  DirectX::XMVECTOR v01 = DirectX::XMVectorSubtract(b_global, a_global);
  DirectX::XMVECTOR v02 = DirectX::XMVectorSubtract(c_global, a_global);
  DirectX::XMVECTOR normal =DirectX::XMVector3Normalize( DirectX::XMVector3Cross(v01, v02));
  return Math::DotProductVector3(normal, DirectX::XMVectorSubtract(vertex_global, a_global)) > 0.0f;
}

void Polytope::AddEdge(size_t a, size_t b)
{
  for (auto it = edges.begin(); it != edges.end(); ++it)
  {
    if (it->a == b && it->b == a)
    {
      edges.erase(it);
      return;
    }
  }
  edges.emplace_back(a, b);
}
} // namespace Octane
