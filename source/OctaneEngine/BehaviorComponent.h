#pragma once

#include <DirectXMath.h>

namespace Octane {

enum class BHVRType
{
	PLAYER,
	WINDTUNNEL,


	INVALID
};

struct BehaviorComponent {
  BHVRType type = BHVRType::INVALID;
  bool initialized = false;
};

}