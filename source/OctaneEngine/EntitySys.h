#pragma once

#include <OctaneEngine/Entity.h>
#include <OctaneEngine/ISystem.h>
#include <OctaneEngine/SystemOrder.h>
#include <EASTL/vector.h>

namespace Octane
{

using EntityID = uint32_t;
static const EntityID INVALID_ENTITY = eastl::numeric_limits<EntityID>::max();

class EntitySys : public ISystem
{
public:
  explicit EntitySys(class Engine* parent_engine);

  typedef GameEntity* Iterator;

  void Load() override;
  void LevelStart() override;
  void Update() override;
  void LevelEnd() override;
  void Unload() override;

  static SystemOrder GetOrder();

  // creates a new entity with 0 components
  EntityID MakeEntity();

  // access an entity
  GameEntity& GetEntity(EntityID which);

  // frees an entity to be re-used later
  void FreeEntity(EntityID which);

  void FreeAllEntities();

  Iterator EntitiesBegin();
  Iterator EntitiesEnd();

private:
  static const SystemOrder ORDER = SystemOrder::Entity;
  // actual memory management not yet implemented, just use a vector for now
  eastl::vector<GameEntity> entities_;

  size_t entity_count_ = 0;
};

} // namespace Octane