/******************************************************************************/
/*!
  \par        Project Octane
  \file       BearBehavior.h
  \author     Brayan Lopez
  \date       2020/10/28
  \brief      stuff enemy behaviors need to know about

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

#pragma once
#include <OctaneEngine/Graphics/Colors.h>
#include <OctaneEngine/Components/TransformComponent.h>
#include <OctaneEngine/Components/PhysicsComponent.h>
namespace Octane
{
  struct EnemyDestroyed //functor
  {
      void operator()();
      //int spawnCap = 20;
      int enemiesSpawned = 0;
      int enemiesKilled = 0;
      int score = 0;
      int highScore = 0;
      int wave = 1;
      int highestWave = 1;
      int enemiesLeft = 0;
      bool spawnedWave = false;
  };

  extern Color HealthColors[5];

  void ChangeEnemyScale(TransformComponent& trans_comp,PhysicsComponent& phys_comp, int health);
}
