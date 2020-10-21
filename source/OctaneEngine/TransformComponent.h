#pragma once

#include <DirectXMath.h>

namespace Octane {
struct TransformComponent {
  DirectX::XMFLOAT3 pos;
  DirectX::XMFLOAT3 scale;
  float rotation;
};
}