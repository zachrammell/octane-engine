#pragma once
#include <OctaneEngine/Physics/Primitive.h>

namespace Octane
{

class Box final : public Primitive
{
public:
  Box();
  ~Box();

  DirectX::XMVECTOR Support(const DirectX::XMVECTOR& direction) override;
  DirectX::XMVECTOR GetNormal(const DirectX::XMVECTOR& local_point) override;
  float GetVolume() override;

private:
  DirectX::XMVECTOR vertices_[8]; //vector3
};

} // namespace Octane
