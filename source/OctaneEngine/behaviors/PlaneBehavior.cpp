#include <OctaneEngine/BehaviorSys.h>
#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/EntitySys.h>
//#include <OctaneEngine/Physics/NarrowPhase.h>
#include <OctaneEngine/Physics/PhysicsSys.h>
#include <OctaneEngine/TransformHelpers.h>
#include <OctaneEngine/behaviors/BearBehavior.h>
#include <OctaneEngine/behaviors/BunnyBehavior.h>
#include <OctaneEngine/behaviors/DuckBehavior.h>
#include <OctaneEngine/behaviors/PlaneBehavior.h>
#include <iostream>

namespace Octane
{
PlaneBehavior::PlaneBehavior(BehaviorSys* parent, ComponentHandle handle, dx::XMVECTOR dir)
  : IBehavior(parent, handle),
    dir_(dir)
{
}

void PlaneBehavior::Initialize()
{

  auto enty = Get<EntitySys>();
  for (auto it = enty->EntitiesBegin(); it != enty->EntitiesEnd(); ++it)
  {
    //check if compared entity is the current plane
    if (it->HasComponent(ComponentKind::Behavior))
    {
      auto other = it->GetComponentHandle(ComponentKind::Behavior);

      if (other == handle_)
      {
        //assume this plane has a physics component
        auto& trans = Get<ComponentSys>()->GetTransform(it->GetComponentHandle(ComponentKind::Transform));
        auto& physics = Get<ComponentSys>()->GetPhysics(it->GetComponentHandle(ComponentKind::Physics));
        dx::XMFLOAT3 facing;
        dx::XMStoreFloat3(&facing, dir_);
        FaceDir(trans, facing);
        physics.SetRotation(trans.rotation);
        //physics.rigid_body.SetGhost(true);
        break;
      }
    }
  }

  //auto& cam = Get<CameraSys>()->GetFPSCamera();
  //auto& trans = Get<ComponentSys>()->GetTransform(trans_handle_);
  //auto& physics = Get<ComponentSys>()->GetPhysics(phys_handle_);
  //ReorientTo(trans, cam.GetInverseOrientation());
  //physics.rigid_body.SetOrientation(trans.rotation);
}

void PlaneBehavior::Update(float dt, EntityID myid)
{
  auto enty = Get<EntitySys>();
  auto phys_sys = Get<PhysicsSys>();
  auto& phys_me = Get<ComponentSys>()->GetPhysics(enty->GetEntity(myid).GetComponentHandle(ComponentKind::Physics));
  auto& trans_me
    = Get<ComponentSys>()->GetTransform(enty->GetEntity(myid).GetComponentHandle(ComponentKind::Transform));
  if (gettingfreed)
  {
   // Get<ComponentSys>()->GetRender(enty->GetEntity(myid).GetComponentHandle(ComponentKind::Render)).render_type
     // = RenderType::Invisible;
    //phys_me.rigid_body.SetStatic();
    return;
  }
  float constexpr G = -9.81f;

  if (lifetime <= 0.f && !gettingfreed)
  {
    Get<EntitySys>()->FreeEntity(myid);
    gettingfreed = true;
    return;
  }
  {
    if (!impulsed)
    {
      auto player = enty->GetPlayer();
      dir_.m128_f32[1] += .005f;
      dir_ = dx::XMVectorScale(dir_, 100.f);
      auto& playerPhys = Get<ComponentSys>()->GetPhysics(player->GetComponentHandle(ComponentKind::Physics));
      auto playerVel = phys_sys->GetVelocity(&playerPhys);
      dir_ = dx::XMVectorAdd(dir_, playerVel);
      dx::XMFLOAT3 force;
      dx::XMStoreFloat3(&force, dir_);

      phys_me.ApplyForce(force);
      dir_ = dx::XMVector3Normalize(dir_);
      dx::XMStoreFloat3(&force, dir_);
      FaceDir(trans_me, force);
      phys_me.SetRotation(trans_me.rotation);

      impulsed = true;
      phys_me.SetGravity(0.5f * G);
      return;
    }
    
    //phys_sys->ApplyForce(&phys_me, {0.f, .15f * G, 0.f});

     //phys_me.ApplyForce({0.f, .15f * G, 0.f});

    dx::XMFLOAT3 vel;
    dx::XMStoreFloat3(&vel,dx::XMVector3Normalize( phys_sys->GetVelocity(&phys_me)));

    FaceDir(trans_me, vel);
    phys_me.SetRotation(trans_me.rotation);

    lifetime -= dt;
  }
  
  
  if (!gettingfreed && phys_sys->HasCollisions(enty->GetEntity(myid).GetComponentHandle(ComponentKind::Physics)))
  {
    for (auto it = enty->EntitiesBegin(); it != enty->EntitiesEnd(); ++it)
    {
      if (it->HasComponent(ComponentKind::Behavior))
      {
        auto other = it->GetComponentHandle(ComponentKind::Behavior);
        const auto& othbeh = Get<ComponentSys>()->GetBehavior(other);

        if (othbeh.type == BHVRType::BEAR || othbeh.type == BHVRType::DUCK || othbeh.type == BHVRType::BUNNY)
        {


          auto& phys_other = it->GetComponentHandle(ComponentKind::Physics);
          

          if (phys_sys->HasCollision(enty->GetEntity(myid).GetComponentHandle(ComponentKind::Physics), phys_other))
          {
            switch (othbeh.type)
            {
            case BHVRType::BEAR:
            {
              BearBehavior* otherBeh = static_cast<BearBehavior*>(
                Get<ComponentSys>()->GetBehavior(it->GetComponentHandle(ComponentKind::Behavior)).behavior);
              otherBeh->TakeDamage();

              break;
            }
            case BHVRType::DUCK:
            {
              DuckBehavior* otherBeh = static_cast<DuckBehavior*>(
                Get<ComponentSys>()->GetBehavior(it->GetComponentHandle(ComponentKind::Behavior)).behavior);
              otherBeh->TakeDamage();

              break;
            }
            case BHVRType::BUNNY:
            {
              BunnyBehavior* otherBeh = static_cast<BunnyBehavior*>(
                Get<ComponentSys>()->GetBehavior(it->GetComponentHandle(ComponentKind::Behavior)).behavior);
              otherBeh->TakeDamage();

              break;
            }
            default: break;
            }

            //prevent freeing multiple times due to multiple collision
            if (!gettingfreed)
            {
              Get<EntitySys>()->FreeEntity(myid);
              gettingfreed = true;
            }
          }

        }
      }
      //else if (enty->GetPlayerID() != it.ID() && it->HasComponent(ComponentKind::Physics)) //check collision with terrain as it should not have a behavior
      //{
      //  auto& phys_other = it->GetComponentHandle(ComponentKind::Physics);

      //  if (phys_sys->HasCollision(enty->GetEntity(myid).GetComponentHandle(ComponentKind::Physics), phys_other))
      //  {
      //    //prevent freeing multiple times due to multiple collision
      //    if (!gettingfreed)
      //    {
      //      Get<EntitySys>()->FreeEntity(myid);
      //      gettingfreed = true;
      //    }
      //  }
      //}

    }
  }
}
void PlaneBehavior::Shutdown() {}

} // namespace Octane
