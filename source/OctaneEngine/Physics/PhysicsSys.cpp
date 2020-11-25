#include <OctaneEngine/Physics/PhysicsSys.h>

#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/FramerateController.h>
#include <OctaneEngine/SystemOrder.h>

#include <OctaneEngine/Trace.h>

namespace Octane
{

PhysicsSys::PhysicsSys(Engine* engine)
  : ISystem(engine),
    broad_phase_ {new btDbvtBroadphase()},
    collision_config_ {new btDefaultCollisionConfiguration()},
    narrow_phase_ {new btCollisionDispatcher(collision_config_)},
    resolution_phase_ {new btSequentialImpulseConstraintSolver()},
    dynamics_world_ {new btDiscreteDynamicsWorld(narrow_phase_, broad_phase_, resolution_phase_, collision_config_)}
{
  dynamics_world_->setGravity(btVector3(0, -10, 0));
  narrow_phase_->setNearCallback(BulletCollisionCallback);
}

DirectX::XMFLOAT3 ToXmFloat3(const btVector3& data)
{
  return DirectX::XMFLOAT3(data.x(), data.y(), data.z());
}
btVector3 ToBtVector3(const DirectX::XMFLOAT3& data)
{
  return btVector3(data.x, data.y, data.z);
}

PhysicsSys::~PhysicsSys()
{
  delete dynamics_world_;
  delete resolution_phase_;
  delete narrow_phase_;
  delete collision_config_;
  delete broad_phase_;
}

void PhysicsSys::LevelStart() {}

void PhysicsSys::Update()
{
  float dt = Get<FramerateController>()->GetDeltaTime();

  entity_collisions_.clear(); // this list is populated in stepSimulation
  dynamics_world_->stepSimulation(dt);

  auto* component_sys = Get<ComponentSys>();
  //copy physics calculation to transform
  for (auto entity = Get<EntitySys>()->EntitiesBegin(); entity != Get<EntitySys>()->EntitiesEnd(); ++entity)
  {
    if (
      entity->active && entity->HasComponent(ComponentKind::Transform) && entity->HasComponent(ComponentKind::Physics))
    {
      auto& transform = component_sys->GetTransform(entity->GetComponentHandle(ComponentKind::Transform));
      auto& physics_component = component_sys->GetPhysics(entity->GetComponentHandle(ComponentKind::Physics));

      if (physics_component.rigid_body)
      {
        btTransform world_transform = physics_component.rigid_body->getWorldTransform();
        auto pos = world_transform.getOrigin();
        auto rot = world_transform.getRotation();
        transform.pos = {pos.x(), pos.y(), pos.z()};
        transform.rotation = {pos.x(), pos.y(), pos.z(), rot.w()};
      }
    }
  }
}

void PhysicsSys::LevelEnd()
{
  //delete remain physics stuff
  for (int i = dynamics_world_->getNumConstraints() - 1; i >= 0; --i)
  {
    dynamics_world_->removeConstraint(dynamics_world_->getConstraint(i));
  }
  for (int i = dynamics_world_->getNumCollisionObjects() - 1; i >= 0; --i)
  {
    btCollisionObject* obj = dynamics_world_->getCollisionObjectArray()[i];

    btRigidBody* body = btRigidBody::upcast(obj);
    if (body && body->getMotionState())
    {
      delete body->getMotionState();
    }

    dynamics_world_->removeCollisionObject(obj);
    delete obj;
  }

  collision_shapes_.clear();
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
    physics_compo->rigid_body = CreateSensor(mass, transform, box_shape);
  }
  else
  {
    //rigid body
    physics_compo->rigid_body = CreateRigidBody(mass, transform, box_shape);
  }
}

void PhysicsSys::SetPosition(PhysicsComponent* compo, const DirectX::XMFLOAT3& position)
{
  btTransform transform = compo->rigid_body->getWorldTransform();
  transform.setOrigin(btVector3(position.x, position.y, position.z));
  compo->rigid_body->setWorldTransform(transform);
}

void PhysicsSys::ApplyForce(PhysicsComponent* compo, const DirectX::XMFLOAT3& force) const
{
  btRigidBody* body = compo->rigid_body;
  if (body && body->getMotionState())
  {
    body->applyCentralForce(btVector3(force.x, force.y, force.z));
  }
}

void PhysicsSys::ApplyTorque(PhysicsComponent* compo, const DirectX::XMFLOAT3& torque) const
{
  btRigidBody* body = compo->rigid_body;
  if (body && body->getMotionState())
  {
    body->applyTorque(btVector3(torque.x, torque.y, torque.z));
  }
}

btRigidBody* PhysicsSys::CreateRigidBody(float mass, const btTransform& transform, btCollisionShape* shape) const
{
  btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

  if (shape != nullptr)
  {
    bool is_dynamic = (mass != 0.f);

    btVector3 local_inertia(0, 0, 0);
    if (is_dynamic)
      shape->calculateLocalInertia(mass, local_inertia);
    btDefaultMotionState* motion_state = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo construction_info(mass, motion_state, shape, local_inertia);
    btRigidBody* body = new btRigidBody(construction_info);
    dynamics_world_->addRigidBody(body);
    return body;
  }

  return nullptr;
}

btRigidBody* PhysicsSys::CreateSensor(float mass, const btTransform& transform, btCollisionShape* shape) const
{
  btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

  if (shape != nullptr)
  {
    bool is_dynamic = (mass != 0.f);

    btVector3 local_inertia(0, 0, 0);
    if (is_dynamic)
      shape->calculateLocalInertia(mass, local_inertia);

    btDefaultMotionState* motion_state = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo construction_info(mass, motion_state, shape, local_inertia);
    btRigidBody* body = new btRigidBody(construction_info);
    dynamics_world_->addRigidBody(body);
    body->setCollisionFlags(body->getCollisionFlags() | btRigidBody::CF_NO_CONTACT_RESPONSE);
    return body;
  }

  return nullptr;
}

void PhysicsSys::BulletCallback(btDynamicsWorld* world, btScalar time_step)
{
  if (world == nullptr)
    return;

  if (world->getWorldUserInfo() == nullptr)
    return;

  PhysicsSys* physics_sys = static_cast<PhysicsSys*>(world->getWorldUserInfo());
  //write collision pair callback.
}

void PhysicsSys::BulletCollisionCallback(
  btBroadphasePair& collisionPair,
  btCollisionDispatcher& dispatcher,
  const btDispatcherInfo& dispatchInfo)
{
  // these are void* and could technically be any object, if we do weird implementation stuff
  btCollisionObject* obj0 = reinterpret_cast<btCollisionObject*>(collisionPair.m_pProxy0->m_clientObject);
  btCollisionObject* obj1 = reinterpret_cast<btCollisionObject*>(collisionPair.m_pProxy1->m_clientObject);

  // getUserIndex returns an arbitrary user-defined value that we set earlier when the component was created
  ComponentHandle id0 = obj0->getUserIndex();
  ComponentHandle id1 = obj1->getUserIndex();

  // now we insert the collisions into the list
  PhysicsSys* phys = GetEngine()->GetSystem<PhysicsSys>();

  // we add in both orderings so one can efficiently lookup the collision by either entityid
  phys->entity_collisions_.insert(eastl::make_pair(id0, id1));
  phys->entity_collisions_.insert(eastl::make_pair(id1, id0));

  // fall back to default physics behavior
  dispatcher.defaultNearCallback(collisionPair, dispatcher, dispatchInfo);
};

PhysicsSys::CollisionsResult PhysicsSys::GetCollisions(EntityID entity)
{
  return entity_collisions_.equal_range_small(entity);
}
bool PhysicsSys::HasCollisions(ComponentHandle phys_comp)
{
  return entity_collisions_.count(phys_comp) != 0;
}

} // namespace Octane
