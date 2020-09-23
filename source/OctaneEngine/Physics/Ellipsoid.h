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

private:
  DirectX::XMVECTOR radius_;
};
} // namespace Octane
