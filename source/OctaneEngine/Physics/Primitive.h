#pragma once
#include <OctaneEngine/Math.h>

namespace Octane
{

class Primitive
{
public:
  Primitive();
  virtual ~Primitive();

  virtual DirectX::XMVECTOR Support(const DirectX::XMVECTOR& direction) = 0;
  virtual DirectX::XMVECTOR GetNormal(const DirectX::XMVECTOR& local_point) = 0;
  virtual float GetVolume() = 0;
  virtual void CalculateMassData(float density) = 0;
  float GetDensity() const;
  void SetMass(float density);
  //void CastRay(CastResult& result, float max_distance = -1.0f);
  //void IntersectRay(IntersectionResult& result, float max_distance = -1.0f);
  //virtual bool TestRay(const Ray& local_ray, float& minimum_t, float& maximum_t) const = 0;

protected:
  float density_ = 1.0f;
  float mass_ = 1.0f;
  DirectX::XMVECTOR local_centroid_; //vector3
  DirectX::XMMATRIX local_inertia_;  //matrix3x3
};

} // namespace Octane
