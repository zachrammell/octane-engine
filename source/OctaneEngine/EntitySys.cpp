#include "EntitySys.h"

#include <iostream> // error logging

namespace Octane {

namespace {
void ResetEntity (GameEntity& ent) {
  ent.active = false;
  for (auto& comp: ent.components) {
    comp = INVALID_COMPONENT;
  }
}
}

SystemOrder EntitySys::GetOrder()
{
  return ORDER;
}
EntitySys::EntitySys(class Engine* parent_engine) : ISystem(parent_engine)
{}

// just performs a linear search for the first free space in the array right now
// may use a proper memory manager later
EntityID EntitySys::MakeEntity()
{
  GameEntity ent;
  ent.active = true;
  for (auto& comp: ent.components) {
    comp = INVALID_COMPONENT;
  }

  entities_.push_back(ent);
  const size_t ent_index = entities_.size() - 1; // index of the newly-added entity
  ++entity_count_;
  return static_cast<EntityID>(ent_index);
}
void EntitySys::FreeEntity(EntityID which) {
  if (!entities_[which].active) {
    std::cerr << "Trying to free entity #" << which << " which is already freed!" << std::endl;
  }
  ResetEntity(entities_[which]);
  --entity_count_;
}

void EntitySys::FreeAllEntities()
{
  entities_.clear();
}

void EntitySys::Load() {}
void EntitySys::LevelStart() {}
void EntitySys::Update() {}
void EntitySys::LevelEnd() {}
void EntitySys::Unload() {}

GameEntity& EntitySys::GetEntity(EntityID which)
{
  return entities_[which];
}
EntitySys::Iterator EntitySys::EntitiesBegin()
{
  return entities_.begin();
}
EntitySys::Iterator EntitySys::EntitiesEnd()
{
  return entities_.end();
}

}