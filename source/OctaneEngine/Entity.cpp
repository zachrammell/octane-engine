#include "Entity.h"

namespace Octane
{

ComponentHandle GameEntity::GetComponentHandle(ComponentKind which)
{
  return components[static_cast<int>(which)];
}

}