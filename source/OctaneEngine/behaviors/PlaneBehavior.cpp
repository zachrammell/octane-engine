#include <OctaneEngine/BehaviorSys.h>
#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/Physics/NarrowPhase.h>
#include <OctaneEngine/Physics/PhysicsSys.h>
#include <OctaneEngine/TransformHelpers.h>
#include <OctaneEngine/behaviors/BearBehavior.h>
#include <OctaneEngine/behaviors/PlaneBehavior.h>

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
        physics.rigid_body.SetOrientation(trans.rotation);
        physics.rigid_body.SetGhost(true);
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
  auto& phys_me = Get<ComponentSys>()->GetPhysics(enty->GetEntity(myid).GetComponentHandle(ComponentKind::Physics));
  auto& trans_me
    = Get<ComponentSys>()->GetTransform(enty->GetEntity(myid).GetComponentHandle(ComponentKind::Transform));
  float constexpr G = -9.81f;

  {
    phys_me.rigid_body.ApplyForceCentroid({0.f, .2f * G, 0.f});

    if (!impulsed)
    {
      dir_.m128_f32[1] += .25f;
      dir_ = dx::XMVectorScale(dir_, 350.f);
      dx::XMFLOAT3 force;
      dx::XMStoreFloat3(&force, dir_);
      phys_me.rigid_body.ApplyForceCentroid(force);
      FaceDir(trans_me, force);
      phys_me.rigid_body.SetOrientation(trans_me.rotation);

      impulsed = true;
      return;
    }

    dx::XMFLOAT3 vel;
    dx::XMStoreFloat3(&vel, dx::XMVector3Normalize(phys_me.rigid_body.GetLinearVelocity()));

    FaceDir(trans_me, vel);
    phys_me.rigid_body.SetOrientation(trans_me.rotation);

    lifetime -= dt;


    if (lifetime <= 0.f && !gettingfreed)
    {
      Get<EntitySys>()->FreeEntity(myid);
      gettingfreed = true;
    }
  }

  {
    for (auto it = enty->EntitiesBegin(); it != enty->EntitiesEnd(); ++it)
    {
      if (it->HasComponent(ComponentKind::Behavior))
      {
        auto other = it->GetComponentHandle(ComponentKind::Behavior);
        const auto& othbeh = Get<ComponentSys>()->GetBehavior(other);

        if (othbeh.type == BHVRType::BEAR)
        {
          BearBehavior* otherBeh = static_cast<BearBehavior*>(
            Get<ComponentSys>()->GetBehavior(it->GetComponentHandle(ComponentKind::Behavior)).behavior);
          auto& phys_other = Get<ComponentSys>()->GetPhysics(it->GetComponentHandle(ComponentKind::Physics));
          auto& trans_other = Get<ComponentSys>()->GetTransform(it->GetComponentHandle(ComponentKind::Transform));

          if (Get<PhysicsSys>()->HasCollision(trans_me, phys_me.primitive, trans_other, phys_other.primitive))
          {
            otherBeh->TakeDamage();
            if (!gettingfreed)
            {
              Get<EntitySys>()->FreeEntity(myid);
              gettingfreed = true;
            }
          }
        }
      }
    }
  }
}
void PlaneBehavior::Shutdown() {}

} // namespace Octane
