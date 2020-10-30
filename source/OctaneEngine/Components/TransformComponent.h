#pragma once

#include <DirectXMath.h>

namespace Octane
{

struct TransformComponent
{
  DirectX::XMFLOAT3 pos;
  DirectX::XMFLOAT3 scale;
  // quaternion rotation
  DirectX::XMFLOAT4 rotation;
};

} // namespace Octane