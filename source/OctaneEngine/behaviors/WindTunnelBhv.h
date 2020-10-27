/******************************************************************************/
/*!
  \par        Project Octane
  \file       WindTunnelBhv.h
  \author     Lowell Novitch
  \date       2020/10/23
  \brief      behavior for wind tunnels

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once

// Includes
#include <OctaneEngine/behaviors/IBehavior.h>

namespace Octane
{

class WindTunnelBHV final: public IBehavior
{
public:
  explicit WindTunnelBHV(BehaviorSys* parent, ComponentHandle handle);

  ~WindTunnelBHV() = default;

   void Initialize() override;
   void Update(float dt) override ;
   void Shutdown() override;

 private:
   ComponentHandle phys_handle_ = INVALID_COMPONENT;
};

}
