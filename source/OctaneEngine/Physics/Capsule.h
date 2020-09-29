#pragma once
#include <OctaneEngine/Physics/Primitive.h>

namespace Octane
{
class Capsule final : public Primitive
{
public:
  Capsule();
  ~Capsule();

  DirectX::XMVECTOR Support(const DirectX::XMVECTOR& direction) override;
  DirectX::XMVECTOR GetNormal(const DirectX::XMVECTOR& local_point) override;
  float GetVolume() override;
  void CalculateMassData(float density) override;

private:
  DirectX::XMVECTOR radius_; //vector3
  float height_ = 1.0f;
};
} // namespace Octane
