#include <OctaneEngine/Physics/PhysicsSys.h>

#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/FramerateController.h>
#include <OctaneEngine/SystemOrder.h>

#include <OctaneEngine/Trace.h>

namespace Octane
{
DirectX::XMFLOAT3 ToXmFloat3(const btVector3& data)
{
  return DirectX::XMFLOAT3(data.x(), data.y(), data.z());
}

btVector3 ToBtVector3(const DirectX::XMFLOAT3& data)
{
  return btVector3(data.x, data.y, data.z);
}

DirectX::XMVECTOR ToXmVector(const btVector3& data)
{
  return DirectX::XMLoadFloat3(&ToXmFloat3(data));
}

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
  dynamics_world_->setInternalTickCallback(BulletCallback);
  dynamics_world_->setWorldUserInfo(this);
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
  auto* component_sys = Get<ComponentSys>();

  // set all object pos/rot based on transform
  for (auto entity = Get<EntitySys>()->EntitiesBegin(); entity != Get<EntitySys>()->EntitiesEnd(); ++entity)
  {
    if (entity->active && entity->HasComponent(ComponentKind::Physics))
    {
      auto const& transform = component_sys->GetTransform(entity->GetComponentHandle(ComponentKind::Transform));
      auto& physics_component = component_sys->GetPhysics(entity->GetComponentHandle(ComponentKind::Physics));

      btTransform& world_transform = physics_component.rigid_body->getWorldTransform();
      auto const& pos = transform.pos;
      auto const& rot = transform.rotation;

      world_transform.setOrigin({pos.x, pos.y, pos.z});
      world_transform.setRotation({rot.x, rot.y, rot.z, rot.w});
    }
  }

  entity_collisions_.clear(); // this list is populated in stepSimulation
  dynamics_world_->stepSimulation(dt);

  //now update all transforms to match physics pos/rot
  for (auto entity = Get<EntitySys>()->EntitiesBegin(); entity != Get<EntitySys>()->EntitiesEnd(); ++entity)
  {
    if (
      entity->active && entity->HasComponent(ComponentKind::Transform) && entity->HasComponent(ComponentKind::Physics))
    {
      auto& transform = component_sys->GetTransform(entity->GetComponentHandle(ComponentKind::Transform));
      auto const& physics_component = component_sys->GetPhysics(entity->GetComponentHandle(ComponentKind::Physics));

      if (physics_component.rigid_body)
      {
        btTransform& world_transform = physics_component.rigid_body->getWorldTransform();
        auto const& pos = world_transform.getOrigin();
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
}

SystemOrder PhysicsSys::GetOrder()
{
  return SystemOrder::PhysicsSys;
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

DirectX::XMVECTOR PhysicsSys::GetVelocity(const PhysicsComponent* compo) const
{
  return ToXmVector(compo->rigid_body->getLinearVelocity());
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

void PhysicsSys::RemoveRigidBody(btRigidBody* rigid_body)
{
  if (rigid_body != nullptr)
  {
    dynamics_world_->removeRigidBody(rigid_body);

    // then remove the pointer from the ongoing contacts list
    /*for (CollisionPairs::iterator it = last_collision_pair_.begin(); it != last_collision_pair_.end();)
  {
    CollisionPairs::iterator next = it;
    ++next;

    if (it->a == rigid_body || it->b == rigid_body)
    {
      last_collision_pair_.erase(it);
    }

    it = next;
  }*/

    //// if the object is a RigidBody (all of ours are RigidBodies, but it's good to be safe)
    //if (btRigidBody* const body = btRigidBody::upcast(rigid_body))
    //{
    //  // delete the components of the object
    //  delete body->getMotionState();
    //  delete body->getCollisionShape();

    //  for (int ii = body->getNumConstraintRefs() - 1; ii >= 0; --ii)
    //  {
    //    btTypedConstraint* const constraint = body->getConstraintRef(ii);
    //    dynamics_world_->removeConstraint(constraint);
    //    delete constraint;
    //  }
    //}

    //delete rigid_body;
  }
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

bool PhysicsSys::HasCollisions(ComponentHandle phys_comp) const
{
  return entity_collisions_.count(phys_comp) != 0;
}

bool PhysicsSys::HasCollision(ComponentHandle lhs, ComponentHandle rhs)
{
  auto iters = GetCollisions(lhs);
  for (auto it = iters.first; it != iters.second; ++it)
  {
    if (it->second == rhs)
    {
      return true;
    }
  }
  return false;
}

eCollisionState PhysicsSys::HasCollision(PhysicsComponent& a, PhysicsComponent& b) const
{
  size_t key = reinterpret_cast<size_t>(a.rigid_body) + reinterpret_cast<size_t>(b.rigid_body);
  auto key_range = collision_data_table_.equal_range(key);

  if (key_range.first != key_range.second)
  {
    for (auto it = key_range.first; it != key_range.second; ++it)
    {
      if (
        ((*it).second.a == a.rigid_body && (*it).second.b == b.rigid_body)
        || ((*it).second.a == b.rigid_body && (*it).second.b == a.rigid_body))
      {
        return (*it).second.state;
      }
    }
  }

  return eCollisionState::None;
}

void PhysicsSys::BulletCallback(btDynamicsWorld* world, btScalar /*time_step*/)
{
  if (world == nullptr)
    return;

  if (world->getWorldUserInfo() == nullptr)
    return;

  /* PhysicsSys* physics_sys = static_cast<PhysicsSys*>(world->getWorldUserInfo());
   btDispatcher* dispatcher = world->getDispatcher();
   for (int i = 0; i < dispatcher->getNumManifolds(); ++i)
   {
     btPersistentManifold* manifold = dispatcher->getManifoldByIndexInternal(i);
     const btRigidBody* body0 = static_cast<const btRigidBody*>(manifold->getBody0());
     const btRigidBody* body1 = static_cast<const btRigidBody*>(manifold->getBody1());
     bool swapped = body0 > body1;
 
     btRigidBody* body_a = const_cast<btRigidBody*>(swapped ? body1 : body0);
     btRigidBody* body_b = const_cast<btRigidBody*>(swapped ? body0 : body1);
 
     size_t key = reinterpret_cast<size_t>(body_a) + reinterpret_cast<size_t>(body_b);
 
     auto key_range = physics_sys->collision_data_table_.equal_range(key);
     if (key_range.first != key_range.second)
     {
       bool found = false;
       for (auto it = key_range.first; it != key_range.second; ++it)
       {
         if (((*it).second.a == body_a && (*it).second.b == body_b))
         {
           found = true;
           if ((*it).second.state == eCollisionState::Start || (*it).second.state == eCollisionState::Persist)
             (*it).second.state = eCollisionState::Persist;
           else
             (*it).second.state = eCollisionState::Start;
           break;
         }
       }
       if (found == false)
       {
         CollisionData data(body_a, body_b, eCollisionState::Start);
         physics_sys->collision_data_table_.emplace(key, data);
         physics_sys->collision_key_table_.emplace(body_a, body_b);
         physics_sys->collision_key_table_.emplace(body_b, body_a);
       }
     }
     else
     {
       CollisionData data(body_a, body_b, eCollisionState::Start);
       physics_sys->collision_data_table_.emplace(key, data);
       physics_sys->collision_key_table_.emplace(body_a, body_b);
       physics_sys->collision_key_table_.emplace(body_b, body_a);
     }
   }
   CollisionPairs removed_pairs;
   eastl::set_difference(
     physics_sys->last_collision_pair_.begin(),
     physics_sys->last_collision_pair_.end(),
     curr_collision_pair.begin(),
     curr_collision_pair.end(),
     removed_pairs.begin());
 
   for (auto it = removed_pairs.begin(); it != removed_pairs.end(); ++it)
   {
     auto state = physics_sys->collision_data_table_.find(*it);
     if (state->second.state == eCollisionState::Persist)
     {
       state->second.state = eCollisionState::End;
     }
     else if (state->second.state == eCollisionState::End)
     {
       physics_sys->collision_data_table_.erase(state);
 
       auto a = it->a;
       auto b = it->b;
 
       auto key_range_a = physics_sys->collision_key_table_.equal_range(a);
       if (key_range_a.first != key_range_a.second)
       {
         for (auto range_it = key_range_a.first; range_it != key_range_a.second;)
         {
           if ((*range_it).second == b)
           {
             physics_sys->collision_key_table_.erase(range_it++);
             break;
           }
           else
           {
             ++range_it;
           }
         }
       }
       auto key_range_b = physics_sys->collision_key_table_.equal_range(b);
       if (key_range_b.first != key_range_b.second)
       {
         for (auto range_it = key_range_b.first; range_it != key_range_b.second;)
         {
           if ((*range_it).second == a)
           {
             physics_sys->collision_key_table_.erase(range_it++);
             break;
           }
           else
           {
             ++range_it;
           }
         }
       }
     }
   }
 
   physics_sys->last_collision_pair_ = curr_collision_pair;*/
}
} // namespace Octane
