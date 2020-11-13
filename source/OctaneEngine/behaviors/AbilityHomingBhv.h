/******************************************************************************/
/*!
  \par        Project Octane
  \file       AbilityHomingBhv.h
  \author     Lowell Novitch
  \date       2020/10/23
  \brief      behavior for homing wind tunnels spawned by the player

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once

// Includes
#include <OctaneEngine/behaviors/IBehavior.h>

namespace Octane
{

class AbilityHomingBhv final : public IBehavior
{
public:
  explicit AbilityHomingBhv(BehaviorSys* parent, ComponentHandle handle);

  ~AbilityHomingBhv() = default;

   void Initialize() override;
   void Update(float dt, EntityID myid) override ;
   void Shutdown() override;

 private:

	//serialize this later
   float life_time_ = 5.0f;

   DirectX::XMFLOAT3 GetForce(EntityID myid);

};

}
