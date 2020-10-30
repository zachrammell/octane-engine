#include <OctaneEngine/ComponentSys.h>

#include <OctaneEngine/SystemOrder.h>

#include <OctaneEngine/behaviors/WindTunnelBhv.h>
#include <OctaneEngine/behaviors/PlaneBehavior.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/Graphics/CameraSys.h>
#include <OctaneEngine/BehaviorSys.h>


namespace Octane
{
void ComponentSys::Load() {}

void ComponentSys::LevelStart() {}

void ComponentSys::Update() {}

void ComponentSys::LevelEnd() {}

void ComponentSys::Unload() {}

SystemOrder ComponentSys::GetOrder()
{
  return SystemOrder::ComponentSys;
}

ComponentSys::ComponentSys(class Engine* parent_engine) : ISystem(parent_engine) {}

void ComponentSys::FreeTransform(ComponentHandle id) {}

void ComponentSys::FreePhysics(ComponentHandle id) {}

void ComponentSys::FreeRender(ComponentHandle id) {}

void ComponentSys::FreeBehavior(ComponentHandle id) {
  BehaviorComponent& beh = GetBehavior(id);
  if (beh.behavior != nullptr)
  {
    delete beh.behavior;
  }
}

void ComponentSys::FreeMetadata(ComponentHandle id) {}

RenderComponent& ComponentSys::GetRender(ComponentHandle id)
{
  assert(id != INVALID_COMPONENT);
  assert(id >= 0);
  assert(id <= render_comps_.size());
  return render_comps_[id];
}

TransformComponent& ComponentSys::GetTransform(ComponentHandle id)
{
  assert(id != INVALID_COMPONENT);
  assert(id >= 0);
  assert(id <= transform_comps_.size());
  return transform_comps_[id];
}

PhysicsComponent& ComponentSys::GetPhysics(ComponentHandle id)
{
  assert(id != INVALID_COMPONENT);
  assert(id >= 0);
  assert(id <= physics_comps_.size());
  return physics_comps_[id];
}

BehaviorComponent& ComponentSys::GetBehavior(ComponentHandle id)
{
  assert(id != INVALID_COMPONENT);
  assert(id >= 0);
  assert(id <= behavior_comps_.size());
  return behavior_comps_[id];
}

MetadataComponent& ComponentSys::GetMetadata(ComponentHandle id)
{
  assert(id != INVALID_COMPONENT);
  assert(id >= 0);
  assert(id <= metadata_comps_.size());
  return metadata_comps_[id];
}

ComponentHandle ComponentSys::MakeRender()
{
  render_comps_.emplace_back(RenderComponent{});
  return static_cast<ComponentHandle>(render_comps_.size() - 1);
}

ComponentHandle ComponentSys::MakeTransform()
{
  transform_comps_.push_back_uninitialized();
  return static_cast<ComponentHandle>(transform_comps_.size() - 1);
}

ComponentHandle ComponentSys::MakePhysics()
{
  physics_comps_.push_back_uninitialized();
  return static_cast<ComponentHandle>(physics_comps_.size() - 1);
}

ComponentHandle ComponentSys::MakeBehavior(BHVRType type)
{
  BehaviorComponent beh = BehaviorComponent();
  beh.type = type;
  switch (beh.type)
  {
  case BHVRType::PLAYER: beh.behavior = new WindTunnelBHV(Get<BehaviorSys>(), static_cast<ComponentHandle>(behavior_comps_.size())); break;
  case BHVRType::WINDTUNNEL:
    beh.behavior = new WindTunnelBHV(Get<BehaviorSys>(), static_cast<ComponentHandle>(behavior_comps_.size()));
    break;
  case BHVRType::PLANE:
  {
    auto& camera = Get<CameraSys>()->GetFPSCamera();
    beh.behavior = new PlaneBehavior(
      Get<BehaviorSys>(),
      static_cast<ComponentHandle>(behavior_comps_.size()),
      camera.GetViewDirection());
  }
  break;

  default: break;
  }

  behavior_comps_.emplace_back(beh);
  return static_cast<ComponentHandle>(behavior_comps_.size() - 1);
}

ComponentHandle ComponentSys::MakeMetadata()
{
  metadata_comps_.emplace_back(MetadataComponent {});
  return static_cast<ComponentHandle>(metadata_comps_.size() - 1);
}

} // namespace Octane
