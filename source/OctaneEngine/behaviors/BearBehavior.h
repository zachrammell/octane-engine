/******************************************************************************/
/*!
  \par        Project Octane
  \file       BearBehavior.h
  \author     Brayan Lopez
  \date       2020/10/28
  \brief      behavior for Bears

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

#pragma once
#include <OctaneEngine/behaviors/IBehavior.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/behaviors/EnemyBehaviorCommon.h>
#include <DirectXMath.h>
namespace dx = DirectX;
namespace Octane
{

class BearBehavior : public IBehavior
{
public:
  explicit BearBehavior(BehaviorSys* parent, ComponentHandle handle, EntityID target);

  virtual ~BearBehavior() = default;

  void Initialize() override;
  void Update(float dt, EntityID myID);
  void Shutdown() override;
  void SetDestroyedFunc(EnemyDestroyed& edfunc);
  void TakeDamage();
  private:
    ComponentHandle phys_handle_ = INVALID_COMPONENT;
    ComponentHandle trans_handle_ = INVALID_COMPONENT;
    ComponentHandle target_trans_handle_ = INVALID_COMPONENT;
    EntityID target_;
    EnemyDestroyed* destroyed_func_ = nullptr;
    int health_ = 1;
    bool gettingFreed = false;
};

}
