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
#include <OctaneEngine/EntityID.h>
#include <EASTL/algorithm.h>
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

Color HealthColors[] = 
{
  Colors::db32[27],
  Colors::db32[18],
  Colors::db32[11],
  Colors::db32[8],
  Colors::db32[29]
};

void ChangeEnemyScale(TransformComponent& trans_comp, PhysicsComponent& phys_comp, int health)
{
  const float a = eastl::max(.15f * (health - 1), 0.f);
  trans_comp.scale = {.35f + a, .35f + a, .35f + a};
  phys_comp.SetScale({trans_comp.scale.x*2.f,trans_comp.scale.y*4.f,trans_comp.scale.z*2.f});
}

} // namespace Octane
