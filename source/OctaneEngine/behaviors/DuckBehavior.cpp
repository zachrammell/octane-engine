#include <OctaneEngine/AudioPlayer.h>
#include <OctaneEngine/BehaviorSys.h>
#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/Graphics/CameraSys.h>
#include <OctaneEngine/TransformHelpers.h>
#include <OctaneEngine/behaviors/DuckBehavior.h>
#include <OctaneEngine/behaviors/TemporaryBehavior.h>

namespace Octane
{
DuckBehavior::DuckBehavior(BehaviorSys* parent, ComponentHandle handle, EntityID targetID)
  : IBehavior(parent, handle),
    target_(targetID)
{
}

void DuckBehavior::Initialize()
{
  auto enty = Get<EntitySys>();
  GameEntity& target = enty->GetEntity(target_);
  target_trans_handle_ = target.GetComponentHandle(ComponentKind::Transform);

  if (target_ == INVALID_ENTITY)
    target_ = enty->GetPlayerID();

  for (auto it = enty->EntitiesBegin(); it != enty->EntitiesEnd(); ++it)
  {
    //check if compared entity is the current plane
    if (it->HasComponent(ComponentKind::Behavior))
    {
      auto other = it->GetComponentHandle(ComponentKind::Behavior);

      if (other == handle_)
      {
        //assume this plane has a physics component
        phys_handle_ = it->GetComponentHandle(ComponentKind::Physics);
        trans_handle_ = it->GetComponentHandle(ComponentKind::Transform);
        if (target_trans_handle_ != INVALID_COMPONENT)
          break;
      }
    }
    if (it.Get() == &target)
    {
      target_trans_handle_ = it->GetComponentHandle(ComponentKind::Transform);
      if (phys_handle_ != INVALID_COMPONENT && trans_handle_ != INVALID_COMPONENT)
        break;
    }
  }
}

void DuckBehavior::Update(float dt, EntityID myID)
{
  if (gettingFreed)
    return;
  float constexpr G = -9.81f;
  if (
    phys_handle_ == INVALID_COMPONENT || trans_handle_ == INVALID_COMPONENT
    || target_trans_handle_ == INVALID_COMPONENT)
    return Initialize();

  auto& trans = Get<ComponentSys>()->GetTransform(trans_handle_);
  auto& physics = Get<ComponentSys>()->GetPhysics(phys_handle_);
  auto& target = Get<ComponentSys>()->GetTransform(target_trans_handle_);
  auto enty = Get<EntitySys>();

  if (health_ <= 0 && destroyed_func_)
  {

    AudioPlayer::Play_Event(AK::EVENTS::ENEMY_DEATH);
    (*destroyed_func_)();
    gettingFreed = true;
    Get<EntitySys>()->FreeEntity(myID);
    return;
  }

  //fake ground
  LockYRelToTarget(trans.pos, {0.f, 0.f, 0.f}, -.25f);
  //move and face target
  if (!dx::XMScalarNearEqual(trans.pos.y, 0.0f, 0.250000f))
  {
    SimpleMove(physics, trans.pos, target.pos, 2.75f);
  }
  else
  {
    SimpleMove(physics, trans.pos, target.pos, 0.45f);
    flyRetry += dt;
  }
  FacePos(trans, target.pos);
  if (flyRetry >= 5.0f)
  {
    if (RandomJump(physics, trans.pos, 0.0f, 20.f * -G))
    {
      flyRetry = rand() % 6 * 1.f;
    }
  }
}
void DuckBehavior::Shutdown() {}

void DuckBehavior::SetDestroyedFunc(EnemyDestroyed& edfunc)
{
  destroyed_func_ = &edfunc;
}

void DuckBehavior::TakeDamage()
{
  --health_;
  auto& renderComp
    = Get<ComponentSys>()->GetRender(Get<EntitySys>()->GetEntity(id_).GetComponentHandle(ComponentKind::Render));
  renderComp.material.diffuse = HealthColors[health_ - 1];
  auto& transComp
    = Get<ComponentSys>()->GetTransform(Get<EntitySys>()->GetEntity(id_).GetComponentHandle(ComponentKind::Transform));
  auto& physComp
    = Get<ComponentSys>()->GetPhysics(Get<EntitySys>()->GetEntity(id_).GetComponentHandle(ComponentKind::Physics));

  ChangeEnemyScale(transComp, physComp, health_);

  int enemy_bits_count = (rand() % 10) + 5;
  for (int i = 0; i < enemy_bits_count; ++i)
  {
    float scale = 0.05f + (rand() / RAND_MAX) * 0.02f;
    EntityID bit_id = Get<EntitySys>()->CreateEntity(transComp.pos, {scale, scale, scale}, {0, 0, 0, 1});
    GameEntity& bit_game_entity = Get<EntitySys>()->GetEntity(bit_id);
    TransformComponent& transform_component
      = Get<ComponentSys>()->GetTransform(bit_game_entity.GetComponentHandle(ComponentKind::Transform));

    ComponentHandle physics_handle
      = Get<ComponentSys>()->MakePhysicsBox(transform_component, {scale, scale, scale}, 1.0f);
    PhysicsComponent& physics_component = Get<ComponentSys>()->GetPhysics(physics_handle);

    dx::XMFLOAT3 force;
    dx::XMStoreFloat3(
      &force,
      dx::XMVectorScale(
        dx::XMVector3Normalize(dx::XMVector3Rotate(
          dx::XMVECTOR {1, 1, 0},
          dx::XMQuaternionRotationAxis(dx::XMVECTOR {0, 1, 0}, dx::XM_2PI * rand() / RAND_MAX))),
        (rand() / RAND_MAX) * 3.0f + 2.0f));
    physics_component.ApplyForce(force);

    Get<EntitySys>()->AddRenderComp(bit_id, HealthColors[health_], "Cube");

    Get<EntitySys>()->AddBehavior(bit_id, BHVRType::TEMPORARY);
    BehaviorComponent& bhv
      = Get<ComponentSys>()->GetBehavior(bit_game_entity.GetComponentHandle(ComponentKind::Behavior));
    static_cast<TemporaryBehavior*>(bhv.behavior)->SetLifetime(5.0f);
  }
}

void DuckBehavior::SetHealth(int health, EntityID id)
{
  health_ = health;
  id_ = id;
  auto& renderComp
    = Get<ComponentSys>()->GetRender(Get<EntitySys>()->GetEntity(id).GetComponentHandle(ComponentKind::Render));
  renderComp.material.diffuse = HealthColors[health_ - 1];
  auto& transComp
    = Get<ComponentSys>()->GetTransform(Get<EntitySys>()->GetEntity(id).GetComponentHandle(ComponentKind::Transform));
  auto& physComp
    = Get<ComponentSys>()->GetPhysics(Get<EntitySys>()->GetEntity(id).GetComponentHandle(ComponentKind::Physics));

  ChangeEnemyScale(transComp, physComp, health_);
}

} // namespace Octane
