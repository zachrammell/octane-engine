
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/Physics/PhysicsSys.h>
#include <OctaneEngine/Graphics/MeshSys.h>
#include <iostream> // error logging

namespace Octane
{
namespace dx = DirectX;
namespace
{
void ResetEntity(GameEntity& ent)
{
  ent.active = false;
  /*for (auto& comp : ent.components)
  {
    comp = INVALID_COMPONENT;
  }*/
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
  ent.cleared = false;
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
void EntitySys::Update()
{
  for (auto& ent : entities_)
  {
    if (!ent.active && !ent.cleared) //if entity has been marked for deletion but has not been cleared yet
    {
      ent.cleared = true;
      Get<ComponentSys>()->FreePhysics(ent.components[to_integral(ComponentKind::Physics)]);
      Get<ComponentSys>()->FreeBehavior(ent.components[to_integral(ComponentKind::Behavior)]);

      for (auto& comp : ent.components)
      {
        comp = INVALID_COMPONENT;
      }
    }
  }
}
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
  return EntityIter(static_cast<EntityID>(entities_.size()), *this);
}

EntityID EntitySys::CreateEntity(dx::XMFLOAT3 pos, dx::XMFLOAT3 scale, dx::XMFLOAT4 rotation)
{
  auto* compsys = Get<ComponentSys>();
  auto id = MakeEntity();
  auto& entity = GetEntity(id);

  ComponentHandle trans_id = compsys->MakeTransform();
  entity.components[to_integral(ComponentKind::Transform)] = trans_id;
  TransformComponent& trans = compsys->GetTransform(trans_id);
  trans.pos = pos;
  trans.scale = scale;
  trans.rotation = rotation;

  return id;
}
RenderComponent& EntitySys::AddRenderComp(EntityID id, Octane::Color color, Mesh_Key mesh)
{
  auto* compsys = Get<ComponentSys>();
  auto* meshsys = Get<MeshSys>();
  auto& entity = GetEntity(id);

  ComponentHandle render_comp_id = compsys->MakeRender();
  entity.components[to_integral(ComponentKind::Render)] = render_comp_id;
  RenderComponent& render_comp = compsys->GetRender(render_comp_id);
  render_comp.mesh_type = mesh;
  render_comp.material = meshsys->Get(mesh)->GetMaterial();
  render_comp.material.diffuse = color;
  return render_comp;
}

void EntitySys::AddBehavior(EntityID id, BHVRType behavior)
{
  auto compsys = Get<ComponentSys>();
  auto& entity = GetEntity(id);

  ComponentHandle behavior_comp_id = compsys->MakeBehavior(behavior);
  entity.components[to_integral(ComponentKind::Behavior)] = behavior_comp_id;
  BehaviorComponent& behavior_comp = compsys->GetBehavior(behavior_comp_id);
}

void EntitySys::SetPlayerID(EntityID id)
{
  player_entity_id_ = id;
}

EntityID EntitySys::GetPlayerID()
{
  return player_entity_id_;
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
EntityIter::EntityIter(EntityID id, EntitySys& sys) : id_(id), sys_(sys) {}

} // namespace Octane