/******************************************************************************/
/*!
  \par        Project Octane
  \file       EnemySpawner.h
  \author     Brayan Lopez
  \date       2020/10/28
  \brief      behavior for Bears

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

#pragma once
#include <OctaneEngine/behaviors/IBehavior.h>
#include <OctaneEngine/behaviors/BearBehavior.h>
#include <OctaneEngine/behaviors/DuckBehavior.h>
#include <OctaneEngine/behaviors/BunnyBehavior.h>
#include <OctaneEngine/EntitySys.h>
#include <DirectXMath.h>
namespace dx = DirectX;
namespace Octane
{

class EnemySpawner : public IBehavior
{
public:
  explicit EnemySpawner(BehaviorSys* parent, ComponentHandle handle);

  virtual ~EnemySpawner() = default;

  void Initialize() override;
  void Update(float dt, EntityID myID);
  void Shutdown() override;

  void EnemyDefeated();
  void EnemySpawner::SetEnemyDestroyedFunc(EnemyDestroyed& enemydestroyedfunc);

  private:
    ComponentHandle phys_handle_ = INVALID_COMPONENT;
    void SpawnEnemy();
    int spawnCap = 20;
    int enemiesSpawned = 0;
    float spawnDelay = 15.0f;
    float spawnTimer = spawnDelay;
    EnemyDestroyed* enemy_destroyed_func = nullptr;
    bool spawning = true;
    bool prevSpawning = false;
};

}
