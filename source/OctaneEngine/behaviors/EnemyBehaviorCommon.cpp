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

#include <OctaneEngine/behaviors/EnemyBehaviorCommon.h>

namespace Octane
{

void EnemyDestroyed::operator()()
{
  --enemiesSpawned;
  ++enemiesKilled;
  score += 10;
  if (score > highScore)
    highScore = score;
}

}
