#include "PlayerMovementControllerSys.h"

#include <OctaneEngine/Graphics/CameraSys.h>
#include <OctaneEngine/InputHandler.h>
#include <OctaneEngine/SystemOrder.h>
#include <OctaneEngine/Engine.h>

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

const float PLAYER_SPEED = 2.5f;

} // namespace

void PlayerMovementControllerSys::Load() {}

PlayerMovementControllerSys::PlayerMovementControllerSys(Engine* engine) : ISystem(engine), player_id_(INVALID_ENTITY)
{
}

PlayerMovementControllerSys::~PlayerMovementControllerSys() {}

void PlayerMovementControllerSys::LevelStart() {
}

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

  auto move_dir = CalcPlayerMoveDir();

  // TODO
  // Get<EntitySys>->GetEntity(player_id_)->GetPhysics()->setvelocity....
}

void PlayerMovementControllerSys::LevelEnd() {
}

void PlayerMovementControllerSys::Unload() {
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

  bool jump = input->KeyPressedOrHeld(KEY_JUMP);

  float v_side = (left & !right) ? 1 : (right & !left) ? -1 : 0;
  float v_fwd = (fwd & !back) ? 1 : (back & !fwd) ? -1 : 0;

  DirectX::XMVECTOR cam_dir = Get<CameraSys>()->GetFPSCamera().GetViewDirection();
  cam_dir.m128_f32[2] = 0; // set z value to 0 to project into xy plane
  const DirectX::XMFLOAT3 left_dir_f3 = {-cam_dir.m128_f32[1], cam_dir.m128_f32[0], 0};
  const DirectX::XMVECTOR left_dir = DirectX::XMLoadFloat3(&left_dir_f3);

  DirectX::XMVECTOR move_dir
    = DirectX::XMVectorAdd(DirectX::XMVectorScale(left_dir, v_side), DirectX::XMVectorScale(cam_dir, v_fwd));

  move_dir = DirectX::XMVectorScale(DirectX::XMVector2Normalize(move_dir), PLAYER_SPEED);

  return move_dir;
}

void PlayerMovementControllerSys::FindPlayerID()
{
  //TODO
  player_id_ = INVALID_ENTITY;
}

} // namespace Octane