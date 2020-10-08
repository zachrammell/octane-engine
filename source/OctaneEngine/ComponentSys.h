#pragma once

#include "ISystem.h"
#include "SystemOrder.h"
#include <EASTL/vector.h>
#include <EASTL/numeric_limits.h>

#include "RenderComponent.h"
#include "TransformComponent.h"

namespace Octane
{

enum class ComponentKind
{
  Transform = 0,
  Render,
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

  // access actual data from id
  RenderComponent& GetRender(ComponentHandle id);
  TransformComponent& GetTransform(ComponentHandle id);

  // currently no-ops, no memory management / re-use is done
  void FreeRender(ComponentHandle id);
  void FreeTransform(ComponentHandle id);

private:
  static const SystemOrder ORDER = SystemOrder::Component;

  eastl::vector<RenderComponent> render_comps_;
  eastl::vector<TransformComponent> transform_comps_;
};

} // namespace Octane