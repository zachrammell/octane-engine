#include <OctaneEngine/ComponentSys.h>

namespace Octane
{

void ComponentSys::Load() {}
void ComponentSys::LevelStart() {}
void ComponentSys::Update() {}
void ComponentSys::LevelEnd() {}
void ComponentSys::Unload() {}

SystemOrder ComponentSys::GetOrder()
{
  return ORDER;
}

ComponentSys::ComponentSys(class Engine* parent_engine) : ISystem(parent_engine) {}

void ComponentSys::FreeTransform(ComponentHandle id) {}
void ComponentSys::FreeRender(ComponentHandle id) {}
void ComponentSys::FreeBehavior(ComponentHandle id) {}

RenderComponent& ComponentSys::GetRender(ComponentHandle id)
{
  return render_comps_[id];
}
TransformComponent& ComponentSys::GetTransform(ComponentHandle id)
{
  return transform_comps_[id];
}
BehaviorComponent& ComponentSys::GetBehavior(ComponentHandle id)
{
  return behavior_comps_[id];
}
ComponentHandle ComponentSys::MakeRender()
{
  render_comps_.push_back_uninitialized();
  return static_cast<ComponentHandle>(render_comps_.size() - 1);
}

ComponentHandle ComponentSys::MakeTransform()
{
  transform_comps_.push_back_uninitialized();
  return static_cast<ComponentHandle>(transform_comps_.size() - 1);
}

ComponentHandle ComponentSys::MakeBehavior()
{
  behavior_comps_.push_back_uninitialized();
  return static_cast<ComponentHandle>(behavior_comps_.size() - 1);
}
} // namespace Octane