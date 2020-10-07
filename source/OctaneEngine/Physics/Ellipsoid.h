#pragma once
#include <OctaneEngine/Physics/Primitive.h>

namespace Octane
{
class Ellipsoid final : public Primitive
{
public:
  Ellipsoid();
  ~Ellipsoid();

  DirectX::XMVECTOR Support(const DirectX::XMVECTOR& direction) override;
  DirectX::XMVECTOR GetNormal(const DirectX::XMVECTOR& local_point) override;
  float GetVolume() override;
  void CalculateMassData(float density) override;

private:
  DirectX::XMVECTOR radius_ = DirectX::XMVectorSet(0.125f, 0.125f, 0.125f, 0.0f);
  ;
};
} // namespace Octane
