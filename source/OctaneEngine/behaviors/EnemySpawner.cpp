#include <OctaneEngine/behaviors/EnemySpawner.h>
#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/BehaviorSys.h>
#include <OctaneEngine/Physics/PhysicsSys.h>
#include <OctaneEngine/Physics/Box.h>
#include <OctaneEngine/TransformHelpers.h>
#include <OctaneEngine/AudioPlayer.h>

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
  auto entsys = Get<EntitySys>();
  auto& me = entsys->GetEntity(myID);
  auto& trans = Get<ComponentSys>()->GetTransform(me.GetComponentHandle(ComponentKind::Transform));
  if (!enemy_destroyed_func)
  {
    return;
  }
  spawning = spawnTimer >= spawnDelay && enemy_destroyed_func->enemiesSpawned < enemy_destroyed_func->spawnCap;

  if (!spawning)
  {
    spawnTimer += dt;
  }
  enemy_destroyed_func->spawnedWave = false;
  if (spawning)
  {
    SpawnEnemy();
    if (!prevSpawning)
    {
      enemy_destroyed_func->spawnedWave = true;
      //Octane::AudioPlayer::Play_Event(AK::EVENTS::ENEMY_SPAWN);
    }

  }
  prevSpawning = spawning;
}
void EnemySpawner::Shutdown()
{

}

void EnemySpawner::EnemyDefeated()
{
  --enemiesSpawned;
}

void EnemySpawner::SetEnemyDestroyedFunc(EnemyDestroyed& enemydestroyedfunc)
{
  enemy_destroyed_func = &enemydestroyedfunc;
}

void EnemySpawner::SpawnEnemy()
{
  auto* entsys = Get<EntitySys>();
  auto* compsys = Get<ComponentSys>();
  auto* physics_sys = Get<PhysicsSys>();

  MeshType mesh = MeshType::INVALID;
  const int enemyType = rand() % 3;

  switch (enemyType)
  {
  case 0: mesh = MeshType::Bear; break;
  case 1: mesh = MeshType::Duck; break;
  case 2: mesh = MeshType::Bunny; break;
  default: break;
  }

  auto id = entsys->CreateEntity({0.0f, 1.0f, 0.0f}, {0.25f, 0.25f, 0.25f}, {});
  auto& entity = entsys->GetEntity(id);
  entsys->AddPhysics(id, ePrimitiveType::Box, {.25f, .25f, .25f});
  entsys->AddRenderComp(id, Colors::db32[rand() % 32], mesh);
  entsys->AddBehavior(id, BHVRType::BEAR);
  auto& beh = Get<ComponentSys>()->GetBehavior(entity.GetComponentHandle(ComponentKind::Behavior));
  BearBehavior* enemybeh = static_cast<BearBehavior*>(beh.behavior);
  enemybeh->SetDestroyedFunc(*enemy_destroyed_func);

  ++enemy_destroyed_func->enemiesSpawned;
  if (enemy_destroyed_func->enemiesSpawned >= enemy_destroyed_func->spawnCap)
    spawnTimer = 0.0f;
}

}
