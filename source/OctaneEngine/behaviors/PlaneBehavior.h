/******************************************************************************/
/*!
  \par        Project Octane
  \file       PlaneBehavior.h
  \author     Brayan Lopez
  \date       2020/10/28
  \brief      behavior for paper airplanes

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

#pragma once
#include <OctaneEngine/behaviors/IBehavior.h>
#include <DirectXMath.h>
namespace dx = DirectX;
namespace Octane
{

class PlaneBehavior : public IBehavior
{
public:
  explicit PlaneBehavior(BehaviorSys* parent, ComponentHandle handle,dx::XMVECTOR dir);

  virtual ~PlaneBehavior() = default;

  void Initialize() override;
  void Update(float dt) override;
  void Shutdown() override;

  private:
    ComponentHandle phys_handle_ = INVALID_COMPONENT;
    ComponentHandle trans_handle_ = INVALID_COMPONENT;
    dx::XMVECTOR dir_;
    float lifetime = 5.0f;
    bool impulsed = false;
};

}
