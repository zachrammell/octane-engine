#pragma once

#include <DirectXMath.h>

namespace Octane {
struct TransformComponent {
  DirectX::XMFLOAT3 pos;
  float scale;
  float rotation;
};
}