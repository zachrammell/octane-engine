#include <OctaneEngine/BehaviorSys.h>
#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/Graphics/CameraSys.h>
#include <OctaneEngine/Physics/PhysicsSys.h>
#include <OctaneEngine/SystemOrder.h>
#include <OctaneEngine/behaviors/AbilityHomingBhv.h>
#include <OctaneEngine/behaviors/AbilityTunnelBhv.h>
#include <OctaneEngine/behaviors/BearBehavior.h>
#include <OctaneEngine/behaviors/BunnyBehavior.h>
#include <OctaneEngine/behaviors/DuckBehavior.h>
#include <OctaneEngine/behaviors/EnemySpawner.h>
#include <OctaneEngine/behaviors/PlaneBehavior.h>
#include <OctaneEngine/behaviors/WindTunnelBhv.h>

namespace Octane
{

namespace
{
} // namespace

void ComponentSys::Load() {}

void ComponentSys::LevelStart() {}

void ComponentSys::Update() {}

void ComponentSys::LevelEnd() {}

void ComponentSys::Unload()
{
  for (auto it = BehaviorBegin(); it != BehaviorEnd(); ++it)
  {
    if (it->behavior != nullptr)
    {
      delete it->behavior;
    }
  }

  behavior_comps_.erase(BehaviorBegin(), BehaviorEnd());
}

SystemOrder ComponentSys::GetOrder()
{
  return SystemOrder::ComponentSys;
}

ComponentSys::ComponentSys(class Engine* parent_engine) : ISystem(parent_engine) {}

void ComponentSys::FreeTransform(ComponentHandle id) {}

void ComponentSys::FreePhysics(ComponentHandle id)
{
  if (id != INVALID_COMPONENT)
  {

    PhysicsComponent& compo = GetPhysics(id);   

    if (compo.rigid_body != nullptr)
    {
      Get<PhysicsSys>()->RemoveRigidBody(compo.rigid_body);
      delete compo.rigid_body;
      compo.rigid_body = nullptr;
    }
  }
}

void ComponentSys::FreeRender(ComponentHandle id) {}

void ComponentSys::FreeBehavior(ComponentHandle id)
{
  if (id != INVALID_COMPONENT)
  {
    BehaviorComponent& beh = GetBehavior(id);
    if (beh.behavior != nullptr)
    {
      delete beh.behavior;
      beh.behavior = nullptr;
    }
  }
}

void ComponentSys::FreeMetadata(ComponentHandle id) {}

RenderComponent& ComponentSys::GetRender(ComponentHandle id)
{
  assert(id != INVALID_COMPONENT);
  assert(id >= 0);
  assert(id <= render_comps_.size());
  return render_comps_[id];
}

TransformComponent& ComponentSys::GetTransform(ComponentHandle id)
{
  assert(id != INVALID_COMPONENT);
  assert(id >= 0);
  assert(id <= transform_comps_.size());
  return transform_comps_[id];
}

PhysicsComponent& ComponentSys::GetPhysics(ComponentHandle id)
{
  assert(id != INVALID_COMPONENT);
  assert(id >= 0);
  assert(id <= physics_comps_.size());
  return physics_comps_[id];
}

BehaviorComponent& ComponentSys::GetBehavior(ComponentHandle id)
{
  assert(id != INVALID_COMPONENT);
  assert(id >= 0);
  assert(id <= behavior_comps_.size());
  return behavior_comps_[id];
}

MetadataComponent& ComponentSys::GetMetadata(ComponentHandle id)
{
  assert(id != INVALID_COMPONENT);
  assert(id >= 0);
  assert(id <= metadata_comps_.size());
  return metadata_comps_[id];
}

ComponentHandle ComponentSys::MakeRender()
{
  render_comps_.emplace_back(RenderComponent {});
  return static_cast<ComponentHandle>(render_comps_.size() - 1);
}

ComponentHandle ComponentSys::MakeTransform()
{
  transform_comps_.emplace_back(TransformComponent {});
  return static_cast<ComponentHandle>(transform_comps_.size() - 1);
}

ComponentHandle ComponentSys::MakePhysicsUninitialized()
{
  physics_comps_.emplace_back(PhysicsComponent {});
  return static_cast<ComponentHandle>(physics_comps_.size() - 1);
}

ComponentHandle
  ComponentSys::MakePhysicsWithShape(const TransformComponent& trans, btCollisionShape* shape, float mass, bool sensor)
{

  /// Create Dynamic Objects
  btTransform transform = btTransform(
    btQuaternion(trans.rotation.x, trans.rotation.y, trans.rotation.z, trans.rotation.w),
    btVector3(trans.pos.x, trans.pos.y, trans.pos.z));
  transform.setIdentity();

  btVector3 localInertia(0, 0, 0);
  if (mass != 0)
  {
    shape->calculateLocalInertia(mass, localInertia);
  }

  PhysicsComponent phys_compo;
  if (sensor)
  {
    phys_compo.rigid_body = Get<PhysicsSys>()->CreateSensor(mass, transform, shape);
  }
  else
  {
    phys_compo.rigid_body = Get<PhysicsSys>()->CreateRigidBody(mass, transform, shape);
  }

  physics_comps_.push_back(phys_compo);
  ComponentHandle comp_handle = static_cast<ComponentHandle>(physics_comps_.size() - 1);

  // activate it so it will actually update its physics
  phys_compo.rigid_body->setActivationState(ACTIVE_TAG);

  // this UserIndex value will be used by the physics system to associate a btRigidBody with a component
  phys_compo.rigid_body->setUserIndex(comp_handle);

  return comp_handle;
}

ComponentHandle ComponentSys::MakePhysicsBox(
  const TransformComponent& trans,
  const DirectX::XMFLOAT3& box_half_size,
  float mass,
  bool sensor)
{
  btBoxShape* box_shape = new btBoxShape(btVector3(box_half_size.x, box_half_size.y, box_half_size.z));
  return MakePhysicsWithShape(trans, box_shape, mass, sensor);
}

ComponentHandle ComponentSys::MakePhysicsSphere(const TransformComponent& trans, float radius, float mass, bool sensor)
{
  btSphereShape* sphere_shape = new btSphereShape(radius);
  return MakePhysicsWithShape(trans, sphere_shape, mass, sensor);
}

ComponentHandle ComponentSys::MakeBehavior(BHVRType type)
{
  BehaviorComponent beh = BehaviorComponent();
  beh.type = type;
  switch (beh.type)
  {
    // case BHVRType::PLAYER: beh.behavior = new WindTunnelBHV(Get<BehaviorSys>(), static_cast<ComponentHandle>(behavior_comps_.size())); break;
  case BHVRType::WINDTUNNEL:
    beh.behavior = new WindTunnelBHV(Get<BehaviorSys>(), static_cast<ComponentHandle>(behavior_comps_.size()));
    break;
  case BHVRType::PLANE:
  {
    auto& camera = Get<CameraSys>()->GetFPSCamera();
    
    beh.behavior = new PlaneBehavior(
      Get<BehaviorSys>(),
      static_cast<ComponentHandle>(behavior_comps_.size()),
      camera.GetViewDirection());
    break;
  }
  case BHVRType::BEAR:
  {
    auto entsys = Get<EntitySys>();
    beh.behavior = new BearBehavior(
      Get<BehaviorSys>(),
      static_cast<ComponentHandle>(behavior_comps_.size()),
      entsys->GetPlayerID());
    break;
  }
  case BHVRType::DUCK:
  {
    auto entsys = Get<EntitySys>();
    beh.behavior = new DuckBehavior(
      Get<BehaviorSys>(),
      static_cast<ComponentHandle>(behavior_comps_.size()),
      entsys->GetPlayerID());
    break;
  }
  case BHVRType::BUNNY:
  {
    auto entsys = Get<EntitySys>();
    beh.behavior = new BunnyBehavior(
      Get<BehaviorSys>(),
      static_cast<ComponentHandle>(behavior_comps_.size()),
      entsys->GetPlayerID());
    break;
  }
  case BHVRType::ENEMYSPAWNER:
    beh.behavior = new EnemySpawner(Get<BehaviorSys>(), static_cast<ComponentHandle>(behavior_comps_.size()));
    break;
  case BHVRType::ABILITYTUNNEL:
  {

    beh.behavior = new AbilityTunnelBhv(Get<BehaviorSys>(), static_cast<ComponentHandle>(behavior_comps_.size()));
    auto& camera = Get<CameraSys>()->GetFPSCamera();
    DirectX::XMVECTOR player_dir = camera.GetViewDirection();
    DirectX::XMVECTOR tunel_force = DirectX::XMVectorScale(
      DirectX::XMVector3Normalize(player_dir),
      20.0f); //using a magic number from ability homing (bad) fix later
    DirectX::XMStoreFloat3(&beh.force, tunel_force);
    beh.force.y = 0;
    break;
  }
  case BHVRType::ABILITYHOMMING:
    beh.behavior = new AbilityHomingBhv(Get<BehaviorSys>(), static_cast<ComponentHandle>(behavior_comps_.size()));
    break;
  default: break;
  }

  behavior_comps_.emplace_back(beh);
  return static_cast<ComponentHandle>(behavior_comps_.size() - 1);
}

ComponentHandle ComponentSys::MakeMetadata()
{
  metadata_comps_.emplace_back(MetadataComponent {});
  return static_cast<ComponentHandle>(metadata_comps_.size() - 1);
}

} // namespace Octane
