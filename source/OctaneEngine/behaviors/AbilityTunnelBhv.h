/******************************************************************************/
/*!
  \par        Project Octane
  \file       AbilityTunnelBhv.h
  \author     Lowell Novitch
  \date       2020/10/23
  \brief      behavior for wind tunnels spawned by the player

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once

// Includes
#include <OctaneEngine/behaviors/IBehavior.h>

namespace Octane
{

class AbilityTunnelBhv final : public IBehavior
{
public:
  explicit AbilityTunnelBhv(BehaviorSys* parent, ComponentHandle handle);

  ~AbilityTunnelBhv() = default;

   void Initialize() override;
   void Update(float dt, EntityID myid) override ;
   void Shutdown() override;

 private:

	//serialize this later
   float life_time_ = 5.0f;

};

}
