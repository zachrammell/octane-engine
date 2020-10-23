#include "PlayerMovementControllerSys.h"

#include <OctaneEngine/Engine.h>
#include <OctaneEngine/Graphics/CameraSys.h>
#include <OctaneEngine/InputHandler.h>
#include <OctaneEngine/SystemOrder.h>

#include <SDL_keycode.h>

namespace Octane
{

namespace
{
const SDL_KeyCode KEY_LEFT = SDLK_a;
const SDL_KeyCode KEY_RIGHT = SDLK_d;
const SDL_KeyCode KEY_FWD = SDLK_w;
const SDL_KeyCode KEY_BACK = SDLK_s;
const SDL_KeyCode KEY_JUMP = SDLK_SPACE;
const SDL_KeyCode KEY_CROUCH = SDLK_LSHIFT;

// these should be serialized later
const float PLAYER_SPEED = 2.5f;
const float PLAYER_CROUCH_SPEED = 1.0f;
const float PLAYER_AIRSTRAFE_MAXSPEED = 3.0f;
const float PLAYER_AIRSTRAFE_ACCEL = 1.5f;

// vertical velocity when starting jump
const float PLAYER_JUMP_VEL = 5.0f;

const DirectX::XMVECTOR ZERO_VEC = {0, 0, 0, 0};

} // namespace

void PlayerMovementControllerSys::Load() {}

PlayerMovementControllerSys::PlayerMovementControllerSys(Engine* engine) : ISystem(engine), player_id_(INVALID_ENTITY)
{
}

PlayerMovementControllerSys::~PlayerMovementControllerSys() {}

void PlayerMovementControllerSys::LevelStart() {}

void PlayerMovementControllerSys::Update()
{
  if (player_id_ == INVALID_ENTITY)
  {
    FindPlayerID(); // try to find player
    if (player_id_ == INVALID_ENTITY)
    {
      return; // still no player, bail out
    }
  }

  bool jump_input = Get<InputHandler>()->KeyPressedOrHeld(KEY_JUMP);
  bool crouch_input = Get<InputHandler>()->KeyPressedOrHeld(KEY_CROUCH);
  DirectX::XMVECTOR move_dir = CalcPlayerMoveDir();

  bool is_moving = DirectX::XMVector2Equal(move_dir, ZERO_VEC);

  MoveState nextstate = movementstate_;

  // TODO new_vel = player.getvelocity()
  DirectX::XMVECTOR new_vel = ZERO_VEC;

  switch (movementstate_)
  {
    // these two use the same logic for now
  case MoveState::CROUCH:
  case MoveState::CROUCHWALK:
    if (is_moving)
    {
      if (crouch_input)
      {
        new_vel = DirectX::XMVectorScale(move_dir, PLAYER_CROUCH_SPEED);
        nextstate = MoveState::CROUCHWALK;
      }
      else
      {
        new_vel = DirectX::XMVectorScale(move_dir, PLAYER_SPEED);
        nextstate = MoveState::RUN;
      }
    }
    else
    {
      if (crouch_input)
      {
        new_vel = ZERO_VEC;
        nextstate = MoveState::CROUCH;
      }
      else
      {
        new_vel = ZERO_VEC;
        nextstate = MoveState::STAND;
      }
    }
    break;
  case MoveState::STAND:
  case MoveState::RUN:
    if (jump_input)
    {
      new_vel = DirectX::XMVectorAdd(
        DirectX::XMVectorScale(move_dir, PLAYER_SPEED),
        DirectX::XMVECTOR {0, PLAYER_JUMP_VEL, 0, 0});
      nextstate = MoveState::JUMP;
    }
    else if (is_moving)
    {
      if (crouch_input)
      {
        new_vel = DirectX::XMVectorScale(move_dir, PLAYER_CROUCH_SPEED);
        nextstate = MoveState::CROUCHWALK;
      }
      else
      {
        new_vel = DirectX::XMVectorScale(move_dir, PLAYER_SPEED);
        nextstate = MoveState::RUN;
      }
    }
    else
    {
      if (crouch_input)
      {
        new_vel = ZERO_VEC;
        nextstate = MoveState::CROUCH;
      }
      else
      {
        new_vel = ZERO_VEC;
        nextstate = MoveState::STAND;
      }
    }
    break;
  case MoveState::JUMP:
    if (/* getplayery <= HACKY_GROUND_LEVEL_CONST */ false)
    {
      if (is_moving)
      {
        if (crouch_input)
        {
          new_vel = DirectX::XMVectorScale(move_dir, PLAYER_CROUCH_SPEED);
          nextstate = MoveState::CROUCHWALK;
        }
        else
        {
          new_vel = DirectX::XMVectorScale(move_dir, PLAYER_SPEED);
          nextstate = MoveState::RUN;
        }
      }
      else
      {
        if (crouch_input)
        {
          new_vel = ZERO_VEC;
          nextstate = MoveState::CROUCH;
        }
        else
        {
          new_vel = ZERO_VEC;
          nextstate = MoveState::STAND;
        }
      }
    }
    else
    {
      auto new_acc = DirectX::XMVectorScale(move_dir, PLAYER_AIRSTRAFE_ACCEL);
      new_vel = DirectX::XMVectorAdd(new_vel, new_acc);

      // clamp new_vel to max airspeed
      float squarespeed = DirectX::XMVector3LengthSq(new_vel).m128_f32[0];
      if (squarespeed > PLAYER_AIRSTRAFE_MAXSPEED * PLAYER_AIRSTRAFE_MAXSPEED) {
        new_vel = DirectX::XMVectorScale(DirectX::XMVector3Normalize(new_vel), PLAYER_AIRSTRAFE_MAXSPEED);
      }
    }
    break;
  }

  if (nextstate != movementstate_)
  {
    ExitState(movementstate_);
    movementstate_ = nextstate;
    EnterState(nextstate);
  }

  // TODO
  // Get<EntitySys>->GetEntity(player_id_)->GetPhysics()->setvelocity (newvel)....
}

void PlayerMovementControllerSys::LevelEnd() {}

void PlayerMovementControllerSys::Unload()
{
  player_id_ = INVALID_ENTITY; // clear player id setting when unloading
}

SystemOrder PlayerMovementControllerSys::GetOrder()
{
  return SystemOrder::PlayerMovementControllerSys;
}

DirectX::XMVECTOR PlayerMovementControllerSys::CalcPlayerMoveDir()
{
  InputHandler* input = Get<InputHandler>();

  bool left = input->KeyPressedOrHeld(KEY_LEFT);
  bool right = input->KeyPressedOrHeld(KEY_RIGHT);
  bool fwd = input->KeyPressedOrHeld(KEY_FWD);
  bool back = input->KeyPressedOrHeld(KEY_BACK);

  float v_side = (left & !right) ? 1.0f : (right & !left) ? -1.0f : 0.0f;
  float v_fwd = (fwd & !back) ? 1.0f : (back & !fwd) ? -1.0f : 0.0f;

  if (v_side == 0 && v_fwd == 0)
  {
    return ZERO_VEC;
  }

  DirectX::XMVECTOR cam_dir = Get<CameraSys>()->GetFPSCamera().GetViewDirection();
  cam_dir.m128_f32[1] = 0; // set y value to 0 to project into xz plane
  const DirectX::XMFLOAT3 left_dir_f3 = {-cam_dir.m128_f32[2], 0, cam_dir.m128_f32[0]};
  const DirectX::XMVECTOR left_dir = DirectX::XMLoadFloat3(&left_dir_f3);

  DirectX::XMVECTOR move_dir
    = DirectX::XMVectorAdd(DirectX::XMVectorScale(left_dir, v_side), DirectX::XMVectorScale(cam_dir, v_fwd));

  move_dir = DirectX::XMVector2Normalize(move_dir);

  return move_dir;
}

void PlayerMovementControllerSys::FindPlayerID()
{
  //TODO
  player_id_ = INVALID_ENTITY;
}

// just no-ops, but can handle animation stuff later
void PlayerMovementControllerSys::ExitState(PlayerMovementControllerSys::MoveState) {}
void PlayerMovementControllerSys::EnterState(PlayerMovementControllerSys::MoveState) {}

} // namespace Octane