#include <OctaneEngine/Entity.h>

namespace Octane
{

ComponentHandle& GameEntity::GetComponentHandle(ComponentKind which)
{
  return components[to_integral(which)];
}

bool GameEntity::HasComponent(ComponentKind which) 
{
  return components[to_integral(which)] != INVALID_COMPONENT;
}
} // namespace Octane