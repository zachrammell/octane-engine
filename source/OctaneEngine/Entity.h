#pragma once

#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Helper.h>

#include <EASTL/array.h>

namespace Octane
{
struct GameEntity
{
  // no constructor here -- this is simply a struct, the EntityManager will initialize it
  bool active;
  eastl::array<ComponentHandle, to_integral(ComponentKind::COUNT)> components;

  ComponentHandle& GetComponentHandle(ComponentKind which);
  bool HasComponent(ComponentKind which) const;
};

}