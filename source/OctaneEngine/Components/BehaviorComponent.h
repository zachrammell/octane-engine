#pragma once
#include <DirectXMath.h>

namespace Octane
{

class IBehavior;

enum class BHVRType
{
 // PLAYER,
  WINDTUNNEL,
  PLANE,
  BEAR,

  INVALID
};

struct BehaviorComponent
{
  BHVRType type = BHVRType::INVALID;
  bool initialized = false;
  IBehavior* behavior = nullptr;
  DirectX::XMFLOAT3 force = {0.f, 0.f, 0.f};
};

} // namespace Octane