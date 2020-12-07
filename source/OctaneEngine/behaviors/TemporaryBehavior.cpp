#include <OctaneEngine/behaviors/TemporaryBehavior.h>

#include <OctaneEngine/Engine.h>

#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/BehaviorSys.h>
#include <OctaneEngine/ComponentSys.h>

namespace Octane
{

TemporaryBehavior::TemporaryBehavior(BehaviorSys* parent, ComponentHandle handle, float lifetime)
  : IBehavior(parent, handle),
    lifetime_(lifetime)
{
}

void TemporaryBehavior::Initialize() {}

void TemporaryBehavior::Update(float dt, EntityID id)
{
  lifetime_ -= dt;
  if (lifetime_ < 0.0f)
  {
    Get<EntitySys>()->FreeEntity(id);
  }
}
void TemporaryBehavior::Shutdown() {}

} // namespace Octane
