#include "ComponentSys.h"

namespace Octane
{

void ComponentSys::Load() {}
void ComponentSys::LevelStart() {}
void ComponentSys::Update() {}
void ComponentSys::LevelEnd() {}
void ComponentSys::Unload() {}

Octane::SystemOrder Octane::ComponentSys::GetOrder()
{
  return ORDER;
}

ComponentSys::ComponentSys(class Engine* parent_engine) : ISystem(parent_engine) {}
}