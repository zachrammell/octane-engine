#pragma once

#include "ComponentSys.h"

namespace Octane
{
struct GameEntity
{
  // no constructor here -- this is simply a struct, the EntityManager will initialize it

  bool active;
  ComponentHandle components[static_cast<int>(ComponentKind::COUNT)];

  ComponentHandle GetComponentHandle(ComponentKind which);
};

}