#include <OctaneEngine/behaviors/BearBehavior.h>
#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/BehaviorSys.h>
#include <OctaneEngine/TransformHelpers.h>
#include <OctaneEngine/Graphics/CameraSys.h>
#include <OctaneEngine/AudioPlayer.h>


namespace Octane
{
BearBehavior::BearBehavior(BehaviorSys* parent, ComponentHandle handle, EntityID targetID)
  : IBehavior(parent, handle),target_(targetID)
{
}

void BearBehavior::Initialize() 
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
        if (target_trans_handle_!=INVALID_COMPONENT)
          break;
      }
    }
    if (it.Get() == &target)
    {
      target_trans_handle_ = it->GetComponentHandle(ComponentKind::Transform);
      if (phys_handle_!=INVALID_COMPONENT && trans_handle_!=INVALID_COMPONENT)
        break;
    }
  }
}

void BearBehavior::Update(float dt, EntityID myID)
{
  if (gettingFreed)
  {
    return;
  }

  float constexpr G = -9.81f;
  if (phys_handle_ == INVALID_COMPONENT ||
      trans_handle_ == INVALID_COMPONENT ||
      target_trans_handle_ == INVALID_COMPONENT)
    return Initialize();

  auto& trans = Get<ComponentSys>()->GetTransform(trans_handle_);
  auto& physics = Get<ComponentSys>()->GetPhysics(phys_handle_);
  auto& target = Get<ComponentSys>()->GetTransform(target_trans_handle_);
  auto enty = Get<EntitySys>();

  if (health_ <= 0 && destroyed_func_)
  {
    Octane::AudioPlayer::Play_Event(AK::EVENTS::ENEMY_DEATH);
    (*destroyed_func_)();
   // Get<ComponentSys>()->GetRender(enty->GetEntity(myID).GetComponentHandle(ComponentKind::Render)).render_type
    //  = RenderType::Invisible;
    //physics.rigid_body.SetStatic();
    gettingFreed = true;
    Get<EntitySys>()->FreeEntity(myID);
    return;
  }

  //fake ground
  LockYRelToTarget(trans.pos, {0.f, 0.f, 0.f}, -.25f);
  //move and face target
  //SimpleMove(physics.rigid_body, trans.pos, target.pos, 1.05f);
  FacePos(trans, target.pos);
  //update position in physics component
  //physics.rigid_body.SetPosition(trans.pos);
  //physics.rigid_body.ApplyForceCentroid({0.f, G, 0.f});



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

} // namespace Octane
