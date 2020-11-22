#pragma once

#include <OctaneEngine/Entity.h>
#include <OctaneEngine/SystemOrder.h>
#include <EASTL/vector.h>
#include <OctaneEngine/Physics/Primitive.h>
namespace Octane
{

using EntityID = uint32_t;
static const EntityID INVALID_ENTITY = eastl::numeric_limits<EntityID>::max();

class EntityIter;

class EntitySys : public ISystem
{
public:
  explicit EntitySys(class Engine* parent_engine);

  typedef EntityIter Iterator;

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

  void SetPlayerID(EntityID id);
  EntityID GetPlayerID();
  // returns null if no player id is set
  GameEntity* GetPlayer();

  // frees an entity to be re-used later
  void FreeEntity(EntityID which);

  void FreeAllEntities();

  Iterator EntitiesBegin();
  Iterator EntitiesEnd();

  //allocates new components and initializes them
  EntityID CreateEntity(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotation);
  void AddRenderComp(EntityID id, Octane::Color color, Mesh_Key mesh);
  void AddPhysics(EntityID id, ePrimitiveType primitive, DirectX::XMFLOAT3 colScale);
  void AddBehavior(EntityID id, BHVRType behavior);

private:
  static const SystemOrder ORDER = SystemOrder::EntitySys;
  // actual memory management not yet implemented, just use a vector for now
  eastl::vector<GameEntity> entities_;

  EntityID player_entity_id_ = INVALID_ENTITY;
  size_t entity_count_ = 0;
};

class EntityIter {
public:
  friend class EntitySys;

  EntityIter(EntityIter const& rhs);
  EntityIter& operator++();

  bool operator==(EntityIter& rhs);
  bool operator!=(EntityIter& rhs);

  GameEntity& operator*();
  GameEntity* operator->();
  GameEntity* Get();

  EntityID ID() const;

private:
  explicit EntityIter(EntityID id, EntitySys& sys);

  EntityID id_;
  EntitySys& sys_;

};

} // namespace Octane