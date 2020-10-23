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
  BHVRType type;
};

}