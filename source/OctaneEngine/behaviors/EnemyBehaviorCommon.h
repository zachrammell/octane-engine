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

namespace Octane
{
  struct EnemyDestroyed //functor
  {
      void operator()();
      //int spawnCap = 20;
      int enemiesSpawned = 0;
      int score = 0;
      int highScore = 0;
      int wave = 1;
      bool spawnedWave = false;
  };
}
