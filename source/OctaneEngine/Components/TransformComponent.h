#pragma once

#include <DirectXMath.h>

namespace Octane
{

struct TransformComponent
{
  DirectX::XMFLOAT3 pos = {0, 0, 0};
  DirectX::XMFLOAT3 scale = {1, 1, 1};
  // quaternion rotation
  DirectX::XMFLOAT4 rotation = {0, 0, 0, 1};
};

} // namespace Octane