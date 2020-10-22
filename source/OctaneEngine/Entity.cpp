#include <OctaneEngine/Entity.h>

namespace Octane
{

ComponentHandle& GameEntity::GetComponentHandle(ComponentKind which)
{
  return components[to_integral(which)];
}

} // namespace Octane