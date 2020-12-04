#pragma once
#include <EASTL/numeric_limits.h>

namespace Octane
{
using EntityID = uint32_t;
static constexpr EntityID INVALID_ENTITY = (eastl::numeric_limits<EntityID>::max)();
} // namespace Octane