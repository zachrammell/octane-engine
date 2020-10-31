#include <OctaneEngine/behaviors/EnemySpawner.h>
#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/BehaviorSys.h>
#include <OctaneEngine/Physics/PhysicsSys.h>
#include <OctaneEngine/Physics/Box.h>
#include <OctaneEngine/TransformHelpers.h>

namespace Octane
{
EnemySpawner::EnemySpawner(BehaviorSys* parent, ComponentHandle handle)
  : IBehavior(parent, handle)
{
}

void EnemySpawner::Initialize() 
{

}

void EnemySpawner::Update(float dt, EntityID myID)
{
  spawnTimer += dt;

  if (spawnTimer >= spawnDelay && enemiesSpawned < spawnCap)
  {
    spawnTimer = 0.0f;
    SpawnEnemy();
  }
}
void EnemySpawner::Shutdown()
{

}

void EnemySpawner::EnemyDefeated()
{
  --enemiesSpawned;
}

void EnemySpawner::SpawnEnemy()
{
  auto* entsys = Get<EntitySys>();
  auto* compsys = Get<ComponentSys>();
  auto* physics_sys = Get<PhysicsSys>();

  auto create_transform = [=](GameEntity& entity, dx::XMFLOAT3 pos, dx::XMFLOAT3 scale) {
    ComponentHandle trans_id = compsys->MakeTransform();
    entity.components[to_integral(ComponentKind::Transform)] = trans_id;
    TransformComponent& trans = compsys->GetTransform(trans_id);
    trans.pos = pos;
    trans.scale = scale;
    trans.rotation = {};
  };

  auto create_rendercomp = [=](GameEntity& entity, Octane::Color color, MeshType mesh) {
    ComponentHandle render_comp_id = compsys->MakeRender();
    entity.components[to_integral(ComponentKind::Render)] = render_comp_id;
    RenderComponent& render_comp = compsys->GetRender(render_comp_id);
    render_comp.color = color;
    render_comp.mesh_type = mesh;
  };

  auto create_physics
    = [=](GameEntity& entity, TransformComponent& trans, ePrimitiveType primitive, dx::XMFLOAT3 colScale) {
        ComponentHandle physics_comp_id = compsys->MakePhysics();
        entity.components[to_integral(ComponentKind::Physics)] = physics_comp_id;
        PhysicsComponent& physics_comp = compsys->GetPhysics(physics_comp_id);
        physics_sys->InitializeRigidBody(physics_comp);
        physics_sys->AddPrimitive(physics_comp, ePrimitiveType::Box);
        static_cast<Box*>(physics_comp.primitive)->SetBox(colScale.x, colScale.y, colScale.z);
        physics_comp.rigid_body.SetPosition(trans.pos);
        trans.rotation = physics_comp.rigid_body.GetOrientation();
      };

  auto create_behavior = [=](GameEntity& entity, BHVRType behavior) {
    ComponentHandle behavior_comp_id = compsys->MakeBehavior(behavior);
    entity.components[to_integral(ComponentKind::Behavior)] = behavior_comp_id;
    BehaviorComponent& behavior_comp = compsys->GetBehavior(behavior_comp_id);
    behavior_comp.type = behavior;
  };

  MeshType mesh = MeshType::INVALID;
  const int enemyType = rand() % 3;

  switch (enemyType)
  {
    case 0: mesh = MeshType::Bear; break;
    case 1: mesh = MeshType::Duck; break;
    case 2: mesh = MeshType::Bunny; break;
    default: break;
  }

  auto id = Get<EntitySys>()->MakeEntity();
  GameEntity& enemy = Get<EntitySys>()->GetEntity((id));

  create_transform(enemy, {0.0f,1.0f,0.0f}, {0.25f, 0.25f, 0.25f});
  TransformComponent& trans = compsys->GetTransform(enemy.components[to_integral(ComponentKind::Transform)]);

  create_rendercomp(enemy, Colors::db32[rand() % 32], mesh);

  create_physics(enemy, trans, ePrimitiveType::Box, {0.25f, 0.25f, 0.25f});

  create_behavior(enemy, BHVRType::BEAR);
}

}
