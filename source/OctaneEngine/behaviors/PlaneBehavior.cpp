#include <OctaneEngine/behaviors/PlaneBehavior.h>
#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/BehaviorSys.h>
#include <OctaneEngine/Physics/NarrowPhase.h>
#include <OctaneEngine/TransformHelpers.h>
#include <OctaneEngine/Graphics/CameraSys.h>


namespace Octane
{
PlaneBehavior::PlaneBehavior(BehaviorSys* parent, ComponentHandle handle, dx::XMVECTOR dir)
  : IBehavior(parent, handle),
    dir_(dir)
{
}

void PlaneBehavior::Initialize() 
{
  

  //for (auto it = enty->EntitiesBegin(); it != enty->EntitiesEnd(); ++it)
  //{
  //  //check if compared entity is the current plane
  //  if (it->HasComponent(ComponentKind::Behavior))
  //  {
  //    auto other = it->GetComponentHandle(ComponentKind::Behavior);

  //    if (other == handle_)
  //    {
  //      //assume this plane has a physics component
  //      phys_handle_ = it->GetComponentHandle(ComponentKind::Physics);
  //      trans_handle_ = it->GetComponentHandle(ComponentKind::Transform);
  //      break;
  //    }
  //  }
  //}

  //auto& cam = Get<CameraSys>()->GetFPSCamera();
  //auto& trans = Get<ComponentSys>()->GetTransform(trans_handle_);
  //auto& physics = Get<ComponentSys>()->GetPhysics(phys_handle_);
  //ReorientTo(trans, cam.GetInverseOrientation());
  //physics.rigid_body.SetOrientation(trans.rotation);

}

void PlaneBehavior::Update(float dt, EntityID myid)
{
  auto enty = Get<EntitySys>();
  auto& physics = Get<ComponentSys>()->GetPhysics(enty->GetEntity(myid).GetComponentHandle(ComponentKind::Physics));
  auto& trans_me
    = Get<ComponentSys>()->GetTransform(enty->GetEntity(myid).GetComponentHandle(ComponentKind::Transform));


  float constexpr G = -9.81f;

  physics.rigid_body.ApplyForceCentroid({0.f, .25f * G, 0.f});

  if (!impulsed)
  {
    dir_.m128_f32[1] += .25f;
    dir_ = dx::XMVectorScale(dir_, 350.f);
    dx::XMFLOAT3 force;
    dx::XMStoreFloat3(&force, dir_);
    physics.rigid_body.ApplyForceCentroid(force);
    impulsed = true;
  }
  //Todo: free the entity
  lifetime -= dt;

  //if (lifetime <= 0.f)
  //{
  //  Get<EntitySys>()->FreeEntity(me->)
  //}
}
void PlaneBehavior::Shutdown()
{

}

}
