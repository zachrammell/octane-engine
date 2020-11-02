/******************************************************************************/
/*!
  \par        Project Octane
  \file       PlayerMovementControllerSys.h
  \author     Dante Chiesa
  \date       2020/10/23
  \brief      Manages the current scene

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once

// Includes
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/ISystem.h>
#include <OctaneEngine/Heath.h>

namespace Octane
{

class PlayerMovementControllerSys : public ISystem
{
public:
  explicit PlayerMovementControllerSys(Engine* engine);
  ~PlayerMovementControllerSys();

  void Load() override;
  void LevelStart() override;
  void Update() override;
  void LevelEnd() override;
  void Unload() override;

  static SystemOrder GetOrder();
  Health& GetHealth() { return playerHP_; }

private:
  DirectX::XMVECTOR CalcPlayerMoveDir();

  enum class MoveState {
    STAND,
    RUN,
    CROUCH,
    CROUCHWALK,
    JUMP
  } movementstate_;

  void ExitState(MoveState);
  void EnterState(MoveState);

  Health playerHP_ = Health(5);
  float i_time = 1.0f;
  bool took_damage = false;
};

} // namespace Octane