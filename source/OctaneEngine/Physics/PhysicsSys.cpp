#include <OctaneEngine/Physics/PhysicsSys.h>

#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/FramerateController.h>
#include <OctaneEngine/SystemOrder.h>

#include <OctaneEngine/Trace.h>

namespace Octane
{
PhysicsSys::PhysicsSys(Engine* engine) : ISystem(engine) {}

void PhysicsSys::LevelStart()
{
  bt_broad_phase_ = new btDbvtBroadphase();
  bt_collision_config_ = new btDefaultCollisionConfiguration();
  bt_narrow_phase_ = new btCollisionDispatcher(bt_collision_config_);
  bt_resolution_phase_ = new btSequentialImpulseConstraintSolver();
  bt_world_
    = new btDiscreteDynamicsWorld(bt_narrow_phase_, bt_broad_phase_, bt_resolution_phase_, bt_collision_config_);
  bt_world_->setGravity(btVector3(0, -10, 0));
}

void PhysicsSys::Update()
{
  float dt = Get<FramerateController>()->GetDeltaTime();

  auto* component_sys = Get<ComponentSys>();
  for (auto entity = Get<EntitySys>()->EntitiesBegin(); entity != Get<EntitySys>()->EntitiesEnd(); ++entity)
  {
    if (entity->active && entity->HasComponent(ComponentKind::Physics))
    {
      auto& physics_component = component_sys->GetPhysics(entity->GetComponentHandle(ComponentKind::Physics));
      physics_component.system = this;
    }
  }

  if (bt_world_)
  {
    bt_world_->stepSimulation(dt);
  }

  //copy physics calculation to transform
  for (auto entity = Get<EntitySys>()->EntitiesBegin(); entity != Get<EntitySys>()->EntitiesEnd(); ++entity)
  {
    if (
      entity->active && entity->HasComponent(ComponentKind::Transform) && entity->HasComponent(ComponentKind::Physics))
    {
      auto& transform = component_sys->GetTransform(entity->GetComponentHandle(ComponentKind::Transform));
      auto& physics_component = component_sys->GetPhysics(entity->GetComponentHandle(ComponentKind::Physics));

      btTransform world_transform = physics_component.rigid_body->getWorldTransform();
      auto pos = world_transform.getOrigin();
      auto rot = world_transform.getRotation();
      transform.pos = {pos.x(), pos.y(), pos.z()};
      transform.rotation = {pos.x(), pos.y(), pos.z(), rot.w()};
    }
  }
}

void PhysicsSys::LevelEnd()
{
  //delete remain physics stuff
  if (bt_world_ != nullptr)
  {
    int i;
    for (i = bt_world_->getNumConstraints() - 1; i >= 0; i--)
    {
      bt_world_->removeConstraint(bt_world_->getConstraint(i));
    }
    for (i = bt_world_->getNumCollisionObjects() - 1; i >= 0; i--)
    {
      btCollisionObject* obj = bt_world_->getCollisionObjectArray()[i];
      btRigidBody* body = btRigidBody::upcast(obj);
      if (body && body->getMotionState())
      {
        delete body->getMotionState();
      }
      bt_world_->removeCollisionObject(obj);
      delete obj;
    }
  }
  //delete collision shapes
  for (int j = 0; j < collision_shapes_.size(); j++)
  {
    btCollisionShape* shape = collision_shapes_[j];
    delete shape;
  }
  collision_shapes_.clear();

  delete bt_world_;
  bt_world_ = nullptr;
  delete bt_resolution_phase_;
  bt_resolution_phase_ = nullptr;
  delete bt_broad_phase_;
  bt_broad_phase_ = nullptr;
  delete bt_narrow_phase_;
  bt_narrow_phase_ = nullptr;
  delete bt_collision_config_;
  bt_collision_config_ = nullptr;
}

SystemOrder PhysicsSys::GetOrder()
{
  return SystemOrder::PhysicsSys;
}

void PhysicsSys::InitializePhysics(PhysicsComponent* physics_compo) {}
} // namespace Octane
