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

      btTransform world_transform = physics_component.collision_object->getWorldTransform();
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

void PhysicsSys::InitializePhysicsBox(
  PhysicsComponent* physics_compo,
  const DirectX::XMFLOAT3& box_half_size,
  const DirectX::XMFLOAT3& position,
  const DirectX::XMFLOAT4& rotation,

  bool is_dynamic,
  bool is_sensor)
{
  btBoxShape* box_shape = new btBoxShape(btVector3(box_half_size.x, box_half_size.y, box_half_size.z));
  collision_shapes_.push_back(box_shape);

  /// Create Dynamic Objects
  btTransform transform = btTransform(
    btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w),
    btVector3(position.x, position.y, position.z));
  transform.setIdentity();

  btScalar mass = is_dynamic ? 1.f : 0.0f;
  btVector3 localInertia(0, 0, 0);
  if (is_dynamic)
    box_shape->calculateLocalInertia(mass, localInertia);

  if (is_sensor)
  {
    //ghost object
    physics_compo->collision_object = CreateSensor(transform, box_shape);
  }
  else
  {
    //rigid body
    physics_compo->collision_object = CreateRigidBody(mass, transform, box_shape);
  }
}

btRigidBody* PhysicsSys::CreateRigidBody(float mass, const btTransform& transform, btCollisionShape* shape) const
{
  btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

  if (shape != nullptr)
  {
    bool is_dynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (is_dynamic)
      shape->calculateLocalInertia(mass, localInertia);
    btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);
    btRigidBody* body = new btRigidBody(cInfo);
    bt_world_->addRigidBody(body);
    return body;
  }

  return nullptr;
}

btGhostObject* PhysicsSys::CreateSensor(const btTransform& transform, btCollisionShape* shape) const
{
  btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));
  btGhostObject* sensor = new btGhostObject();
  sensor->setCollisionShape(shape);
  sensor->setWorldTransform(transform);
  bt_world_->addCollisionObject(sensor);
  return sensor;
}
} // namespace Octane
