#include <OctaneEngine/behaviors/BearBehavior.h>
#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/BehaviorSys.h>
#include <OctaneEngine/Physics/NarrowPhase.h>
#include <OctaneEngine/TransformHelpers.h>
#include <OctaneEngine/Graphics/CameraSys.h>
#include <OctaneEngine/Audio.h>


namespace Octane
{
BearBehavior::BearBehavior(BehaviorSys* parent, ComponentHandle handle, GameEntity* target)
  : IBehavior(parent, handle),target_(target)
{
}

void BearBehavior::Initialize() 
{
  auto enty = Get<EntitySys>();

  if (!target_)
    target_ = enty->GetPlayer();

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
        if (target_trans_handle_!=INVALID_COMPONENT)
          break;
      }
    }
    if (it.Get() == target_)
    {
      target_trans_handle_ = it->GetComponentHandle(ComponentKind::Transform);
      if (phys_handle_!=INVALID_COMPONENT && trans_handle_!=INVALID_COMPONENT)
        break;
    }
  }
}

void BearBehavior::Update(float dt, EntityID myID)
{
  float constexpr G = -9.81f;
  if (phys_handle_ == INVALID_COMPONENT ||
      trans_handle_ == INVALID_COMPONENT ||
      target_trans_handle_ == INVALID_COMPONENT)
    return Initialize();

  auto& trans = Get<ComponentSys>()->GetTransform(trans_handle_);
  auto& physics = Get<ComponentSys>()->GetPhysics(phys_handle_);
  auto& target = Get<ComponentSys>()->GetTransform(target_trans_handle_);

  //fake ground
  LockYRelToTarget(trans.pos, {0.f, 0.f, 0.f}, -.25f);
  //move and face target
  SimpleMove(physics.rigid_body, trans.pos, target.pos, 1.05f);
  FacePos(trans, target.pos);
  RandomJump(physics.rigid_body, trans.pos, 0.35f, 20.f * -G);
  //update position in physics component
  physics.rigid_body.SetPosition(trans.pos);
  physics.rigid_body.ApplyForceCentroid({0.f, G, 0.f});

  if (health_ <= 0 && destroyed_func_)
  {
    Octane::AudioPlayer::Play_Event(AK::EVENTS::ENEMY_DEATH);
    (*destroyed_func_)();
    Get<EntitySys>()->FreeEntity(myID);
  }

}
void BearBehavior::Shutdown()
{

}

void BearBehavior::SetDestroyedFunc(EnemyDestroyed& edfunc)
{
  destroyed_func_ = &edfunc;
}

void BearBehavior::TakeDamage()
{
  --health_;
}

void EnemyDestroyed::operator()()
{
  --enemiesSpawned;
  score += 10;
  if (score > highScore)
    highScore = score;
}

} // namespace Octane
