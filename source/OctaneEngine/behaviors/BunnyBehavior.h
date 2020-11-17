/******************************************************************************/
/*!
  \par        Project Octane
  \file       BearBehavior.h
  \author     Brayan Lopez
  \date       2020/10/28
  \brief      behavior for Bunny

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

class BunnyBehavior : public IBehavior
{
public:
  explicit BunnyBehavior(BehaviorSys* parent, ComponentHandle handle, EntityID target);

  virtual ~BunnyBehavior() = default;

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
    dx::XMFLOAT3 target {};
    float switch_dir_timer = 0.0f;
    int health_ = 1;
};

}
