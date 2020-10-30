#pragma once

#include <OctaneEngine/ISystem.h>
#include <OctaneEngine/Components/BehaviorComponent.h>
#include <OctaneEngine/Components/PhysicsComponent.h>
#include <OctaneEngine/Components/RenderComponent.h>
#include <OctaneEngine/Components/TransformComponent.h>
#include <OctaneEngine/Components/MetadataComponent.h>

#include <EASTL/numeric_limits.h>
#include <EASTL/vector.h>

namespace Octane
{

enum class ComponentKind : int32_t
{
  INVALID = -1,
  Transform = 0,
  Render,
  Physics,
  Behavior,
  Metadata,
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
  ComponentHandle MakePhysics();
  ComponentHandle MakeBehavior();
  ComponentHandle MakeMetadata();

  // access actual data from id
  RenderComponent& GetRender(ComponentHandle id);
  TransformComponent& GetTransform(ComponentHandle id);
  PhysicsComponent& GetPhysics(ComponentHandle id);
  BehaviorComponent& GetBehavior(ComponentHandle id);
  MetadataComponent& GetMetadata(ComponentHandle id);

  // currently no-ops, no memory management / re-use is done
  void FreeRender(ComponentHandle id);
  void FreeTransform(ComponentHandle id);
  void FreePhysics(ComponentHandle id);
  void FreeBehavior(ComponentHandle id);
  void FreeMetadata(ComponentHandle id);

  template<typename T>
  using const_iter = typename eastl::vector<T>::const_iterator;
  template<typename T>
  using iter = typename eastl::vector<T>::iterator;

  const_iter<RenderComponent> RenderBegin() const { return render_comps_.cbegin(); }
  const_iter<RenderComponent> RenderEnd() const { return render_comps_.cend(); }
  const_iter<TransformComponent> TransformBegin() const { return transform_comps_.cbegin(); }
  const_iter<TransformComponent> TransformEnd() const { return transform_comps_.cend(); }
  iter<PhysicsComponent> PhysicsBegin() { return physics_comps_.begin(); }
  iter<PhysicsComponent> PhysicsEnd() { return physics_comps_.end(); }
  const_iter<MetadataComponent> MetadataBegin() const { return metadata_comps_.cbegin(); }
  const_iter<MetadataComponent> MetadataEnd() const { return metadata_comps_.cend(); }


private:
  eastl::vector<RenderComponent> render_comps_;
  eastl::vector<TransformComponent> transform_comps_;
  eastl::vector<PhysicsComponent> physics_comps_;
  eastl::vector<BehaviorComponent> behavior_comps_;
  eastl::vector<MetadataComponent> metadata_comps_;
};

} // namespace Octane
