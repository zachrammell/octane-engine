#pragma once

#include <OctaneEngine/ISystem.h>
#include <OctaneEngine/SystemOrder.h>
#include <OctaneEngine/RenderComponent.h>
#include <OctaneEngine/TransformComponent.h>
#include <OctaneEngine/BehaviorComponent.h>

#include <EASTL/numeric_limits.h>
#include <EASTL/vector.h>

namespace Octane
{

enum class ComponentKind
{
  Transform = 0,
  Render,
  Behavior,
  COUNT,
};

typedef uint32_t ComponentHandle;
const ComponentHandle INVALID_COMPONENT = eastl::numeric_limits<ComponentHandle>::max();

class ComponentSys : public ISystem
{
public:
  explicit ComponentSys(class Engine* parent_engine);

  void Load() override;
  void LevelStart() override;
  void Update() override;
  void LevelEnd() override;
  void Unload() override;

  static SystemOrder GetOrder();

  // not sure how to implement templatized type-specific component getting yet..
  // this is a WIP interface

  // allocate a new component
  // does not initialize memory, it will be garbage
  ComponentHandle MakeRender();
  ComponentHandle MakeTransform();
  ComponentHandle MakeBehavior();

  // access actual data from id
  RenderComponent& GetRender(ComponentHandle id);
  TransformComponent& GetTransform(ComponentHandle id);
  BehaviorComponent& GetBehavior(ComponentHandle id);

  // currently no-ops, no memory management / re-use is done
  void FreeRender(ComponentHandle id);
  void FreeTransform(ComponentHandle id);
  void FreeBehavior(ComponentHandle id);

  eastl::vector<RenderComponent>::const_iterator RenderBegin() const { return render_comps_.cbegin(); }
  eastl::vector<RenderComponent>::const_iterator RenderEnd() const { return render_comps_.cend(); }
  eastl::vector<TransformComponent>::const_iterator TransformBegin() const { return transform_comps_.cbegin(); }
  eastl::vector<TransformComponent>::const_iterator TransformEnd() const { return transform_comps_.cend(); }

private:
  static const SystemOrder ORDER = SystemOrder::Component;

  eastl::vector<RenderComponent> render_comps_;
  eastl::vector<TransformComponent> transform_comps_;
  eastl::vector<BehaviorComponent> behavior_comps_;
};

} // namespace Octane