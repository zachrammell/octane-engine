#include <OctaneEngine/BehaviorSys.h>
#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/Physics/PhysicsSys.h>
#include <OctaneEngine/behaviors/EnemySpawner.h>
#include <OctaneEngine/AudioPlayer.h>
#include <OctaneEngine/TransformHelpers.h>

namespace Octane
{
EnemySpawner::EnemySpawner(BehaviorSys* parent, ComponentHandle handle) : IBehavior(parent, handle) {}

void EnemySpawner::Initialize() {}

void EnemySpawner::Update(float dt, EntityID myID)
{
  auto entsys = Get<EntitySys>();
  auto& me = entsys->GetEntity(myID);
  auto& trans = Get<ComponentSys>()->GetTransform(me.GetComponentHandle(ComponentKind::Transform));
  if (!enemy_destroyed_func)
  {
    return;
  }
  spawning = spawnTimer >= spawnDelay && enemy_destroyed_func->enemiesSpawned < spawnCap
             && totalSpawnedCurrrentWave < waveSpawnCap;
  enemy_destroyed_func->enemiesLeft = waveSpawnCap - enemy_destroyed_func->enemiesKilled;
  enemy_destroyed_func->spawnedWave = false;

  if (totalSpawnedCurrrentWave == waveSpawnCap && !enemy_destroyed_func->enemiesSpawned)
  {
    waveTimer += dt;
    if (waveTimer >= waveDelay)
    {
      waveTimer = 0.f;
      totalSpawnedCurrrentWave = 0;
      ++enemy_destroyed_func->wave;
      enemy_destroyed_func->spawnedWave = true;
      enemy_destroyed_func->enemiesKilled = 0;
      if(enemy_destroyed_func->highestWave < enemy_destroyed_func->wave)
      {
        enemy_destroyed_func->highestWave = enemy_destroyed_func->wave;
      }
    }
  }

  if (!spawning)
  {
    spawnTimer += dt;
  }
  if (spawning)
  {
    SpawnEnemy();
    spawnTimer = 0.f;
    //if (!prevSpawning)
    //{
    //
    //  enemy_destroyed_func->spawnedWave = true;
    //  //Octane::AudioPlayer::Play_Event(AK::EVENTS::ENEMY_SPAWN);
    //}
  }
  prevSpawning = spawning;
}
void EnemySpawner::Shutdown() {}

void EnemySpawner::SetEnemyDestroyedFunc(EnemyDestroyed& enemydestroyedfunc)
{
  enemy_destroyed_func = &enemydestroyedfunc;
}

void EnemySpawner::SpawnEnemy()
{
  auto* entsys = Get<EntitySys>();
  auto* compsys = Get<ComponentSys>();
  auto* physics_sys = Get<PhysicsSys>();
  auto player = entsys->GetPlayer();
  auto& player_transform = compsys->GetTransform(player->GetComponentHandle(ComponentKind::Transform));
  Mesh_Key mesh;
  const int enemyType = rand() % 3;
  dx::XMFLOAT3 scale {0.35f, 0.35f, 0.35f};
  auto id = entsys->CreateEntity(
    {rand() % 28 - 14.f, 5.0f, rand() % 28 - 14.f},
    scale,
    {0.f,0.f,0.f,1.f});
  auto& entity = entsys->GetEntity(id);

  auto enemy_transform_handle = entity.components[to_integral(ComponentKind::Transform)];
  auto& enemy_transform = compsys->GetTransform(enemy_transform_handle);
  ComponentHandle physics_component_handle = compsys->MakePhysicsBox(enemy_transform, scale, 1.0f);
  entity.components[to_integral(ComponentKind::Physics)] = physics_component_handle;
  BHVRType behavior = BHVRType::INVALID;
  switch (enemyType)
  {
  case 0:
    mesh = "Bear";
    behavior = BHVRType::BEAR;
    break;
  case 1:
    mesh = "Duck";
    behavior = BHVRType::DUCK;
    break;
  case 2:
    mesh = "Bunny";
    behavior = BHVRType::BUNNY;
    break;
  default: break;
  }
  int health = eastl::min(rand() % (enemy_destroyed_func->wave + 1),6);
  entsys->AddRenderComp(id, Colors::db32[health], mesh);
  entsys->AddBehavior(id, behavior);
  auto& beh = Get<ComponentSys>()->GetBehavior(entity.GetComponentHandle(ComponentKind::Behavior));

  switch (behavior)
  {
  case BHVRType::BEAR:
  {
    auto enemybeh = static_cast<BearBehavior*>(beh.behavior);
    enemybeh->SetDestroyedFunc(*enemy_destroyed_func);
    enemybeh->SetHealth(health, id);
    break;
  }
  case BHVRType::DUCK:
  {
    auto enemybeh = static_cast<DuckBehavior*>(beh.behavior);
    enemybeh->SetDestroyedFunc(*enemy_destroyed_func);
    enemybeh->SetHealth(health, id);
    break;
  }
  case BHVRType::BUNNY:
  {
    auto enemybeh = static_cast<BunnyBehavior*>(beh.behavior);
    enemybeh->SetDestroyedFunc(*enemy_destroyed_func);
    enemybeh->SetHealth(health, id);
    break;
  }
  }

  ++totalSpawnedCurrrentWave;
  ++enemy_destroyed_func->enemiesSpawned;
  if (enemy_destroyed_func->enemiesSpawned >= spawnCap)
    spawnTimer = 0.0f;
}

} // namespace Octane
