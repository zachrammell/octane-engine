#pragma once
#include <OctaneEngine/Physics/Primitive.h>

namespace Octane
{

class Truncated final : public Primitive
{
public:
  Truncated();
  ~Truncated();

  DirectX::XMVECTOR Support(const DirectX::XMVECTOR& direction) override;
  DirectX::XMVECTOR GetNormal(const DirectX::XMVECTOR& local_point) override;
  float GetVolume() override;
  void CalculateMassData(float density) override;

private:
  DirectX::XMVECTOR radius_ = DirectX::XMVectorSet(0.3f, 0.5f, 0.0f, 0.0f); //vector2
  float height_ = 1.0f;
  float ratio_ = 0.5f; //0.0 is cone, 1.0f is cylinder, also it must be positive
};
} // namespace Octane
