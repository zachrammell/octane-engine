#pragma once

namespace Octane
{

class IBehavior;

enum class BHVRType
{
  PLAYER,
  WINDTUNNEL,
  PLANE,

  INVALID
};

struct BehaviorComponent
{
  BHVRType type = BHVRType::INVALID;
  bool initialized = false;
  IBehavior* behavior = nullptr;
};

} // namespace Octane