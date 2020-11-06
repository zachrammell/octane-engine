#include <OctaneEngine/EntitySys.h>

#include <iostream> // error logging

namespace Octane
{

namespace
{
void ResetEntity(GameEntity& ent)
{
  ent.active = false;
  for (auto& comp : ent.components)
  {
    comp = INVALID_COMPONENT;
  }
}
} // namespace

SystemOrder EntitySys::GetOrder()
{
  return ORDER;
}
EntitySys::EntitySys(class Engine* parent_engine) : ISystem(parent_engine) {}

// just performs a linear search for the first free space in the array right now
// may use a proper memory manager later
EntityID EntitySys::MakeEntity()
{
  GameEntity ent;
  ent.active = true;
  for (auto& comp : ent.components)
  {
    comp = INVALID_COMPONENT;
  }

  entities_.push_back(ent);
  const size_t ent_index = entities_.size() - 1; // index of the newly-added entity
  ++entity_count_;
  return static_cast<EntityID>(ent_index);
}
void EntitySys::FreeEntity(EntityID which)
{
  if (!entities_[which].active)
  {
    std::cerr << "Trying to free entity #" << which << " which is already freed!" << std::endl;
  }
  ResetEntity(entities_[which]);
  --entity_count_;
}

void EntitySys::FreeAllEntities()
{
  entities_.clear();
  entity_count_ = 0;
}

void EntitySys::Load() {}
void EntitySys::LevelStart() {}
void EntitySys::Update() {}
void EntitySys::LevelEnd() {}

void EntitySys::Unload()
{
  SetPlayerID(INVALID_ENTITY);
}

GameEntity& EntitySys::GetEntity(EntityID which)
{
  return entities_[which];
}
EntitySys::Iterator EntitySys::EntitiesBegin()
{
  return EntityIter(0, *this);
}
EntitySys::Iterator EntitySys::EntitiesEnd()
{
  return EntityIter(entities_.size(), *this);
}
void EntitySys::SetPlayerID(EntityID id)
{
  player_entity_id_ = id;
}

// Gets the entity set as player, or nullptr if no such entity exists
GameEntity* EntitySys::GetPlayer()
{
  if (player_entity_id_ == INVALID_ENTITY)
  {
    return nullptr;
  }
  else
  {
    return &GetEntity(player_entity_id_);
  }
}

EntityIter::EntityIter(const EntityIter& rhs) : id_(rhs.id_), sys_(rhs.sys_) {}

EntityIter& EntityIter::operator++()
{
  ++id_;
  return *this;
}

bool EntityIter::operator==(EntityIter& rhs)
{
  return id_ == rhs.id_;
}
bool EntityIter::operator!=(EntityIter& rhs)
{
  return id_ != rhs.id_;
}
GameEntity& EntityIter::operator*()
{
  return sys_.GetEntity(id_);
}
GameEntity* EntityIter::operator->()
{
  return &operator*();
}
GameEntity* EntityIter::Get()
{
  return &operator*();
}

EntityID EntityIter::ID() const
{
  return id_;
}
EntityIter::EntityIter(EntityID id, EntitySys& sys) :id_(id), sys_(sys) {}


} // namespace Octane