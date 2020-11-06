#include <OctaneEngine/PlayerMovementControllerSys.h>

#include <OctaneEngine/Engine.h>
#include <OctaneEngine/Graphics/CameraSys.h>
#include <OctaneEngine/InputHandler.h>
#include <OctaneEngine/SystemOrder.h>
#include <OctaneEngine/SceneSys.h>
#include <OctaneEngine/Trace.h>
#include <OctaneEngine/Physics/PhysicsSys.h>
#include <OctaneEngine/AudioPlayer.h>
#include "FramerateController.h"
#include <SDL_keycode.h>
#include <iostream>

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
const float PLAYER_SPEED = 7.0f;
const float PLAYER_CROUCH_SPEED = 3.0f;
const float PLAYER_AIRSTRAFE_MAXSPEED = 12.0f;
const float PLAYER_AIRSTRAFE_ACCEL = 4.5f;
const float PLAYER_GRAVITY_ACCEL = 9.81f;
const float PLAYER_I_TIME = 1.0f;
const int PLAYER_MAX_HP = 5;

// vertical velocity when starting jump
const float PLAYER_JUMP_VEL = 9.0f;

const DirectX::XMVECTOR ZERO_VEC = {0, 0, 0, 0};

const float HACKY_GROUND_Y_LEVEL = 0.5f;

bool isPlayerCollidingWithGround(PhysicsComponent const& player_physics) {
  return player_physics.rigid_body.GetPosition().y <= HACKY_GROUND_Y_LEVEL;
}

} // namespace

void PlayerMovementControllerSys::Load() {
  movementstate_ = MoveState::STAND;
}

PlayerMovementControllerSys::PlayerMovementControllerSys(Engine* engine) : ISystem(engine), movementstate_(MoveState::STAND) {}

PlayerMovementControllerSys::~PlayerMovementControllerSys() {}

void PlayerMovementControllerSys::LevelStart() 
{
  playerHP_.set_maxHP(PLAYER_MAX_HP);
  playerHP_.setCurrentHP(PLAYER_MAX_HP);
  took_damage = false;
}

void PlayerMovementControllerSys::Update()
{
  const float dt = Get<FramerateController>()->GetDeltaTime();
  if (dt == 0) {
    return;
  }
  GameEntity* const player = Get<EntitySys>()->GetPlayer();
  if (!player)
  {
    return; // no player exists, bail out
  }


  const ComponentHandle phys_id = player->GetComponentHandle(ComponentKind::Physics);
  PhysicsComponent& player_physics = Get<ComponentSys>()->GetPhysics(phys_id);

  const bool jump_input = Get<InputHandler>()->KeyPressedOrHeld(KEY_JUMP);
  const bool crouch_input = Get<InputHandler>()->KeyPressedOrHeld(KEY_CROUCH);
  const DirectX::XMVECTOR move_dir = CalcPlayerMoveDir();

  const bool is_moving = !DirectX::XMVector3Equal(move_dir, ZERO_VEC);

  const bool on_ground = isPlayerCollidingWithGround(player_physics);

  MoveState nextstate = movementstate_;

  const DirectX::XMVECTOR old_vel = player_physics.rigid_body.GetLinearVelocity();
  DirectX::XMVECTOR new_vel = old_vel;

  switch (movementstate_)
  {
    // these two use the same logic for now
  case MoveState::CROUCH:
  case MoveState::CROUCHWALK:
    if (!on_ground) {
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
  case MoveState::STAND:
  case MoveState::RUN:
    if (!on_ground) {
      nextstate = MoveState::JUMP;
    }
    else if (jump_input)
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
    if (on_ground)
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
      new_acc.m128_f32[1] = -PLAYER_GRAVITY_ACCEL;
      new_vel = DirectX::XMVectorAdd(new_vel, DirectX::XMVectorScale(new_acc, dt));

      // clamp new_vel to max airspeed
      float squarespeed = DirectX::XMVector3LengthSq(new_vel).m128_f32[0];
      if (squarespeed > PLAYER_AIRSTRAFE_MAXSPEED * PLAYER_AIRSTRAFE_MAXSPEED)
      {
        new_vel = DirectX::XMVectorScale(DirectX::XMVector3Normalize(new_vel), PLAYER_AIRSTRAFE_MAXSPEED);
      }
    }
    break;
  }

  //damage stuff
  
    if(took_damage)
    {
        i_time -= dt;

        if(i_time <= 0.f)
        {
            took_damage = false;
        }
    }
    else
    {
      auto enty = Get<EntitySys>();
      auto& trans_plyr = Get<ComponentSys>()->GetTransform(player->GetComponentHandle(ComponentKind::Transform));
        for(auto it = enty->EntitiesBegin(); it != enty->EntitiesEnd(); ++it)
        {
            if(it->HasComponent(ComponentKind::Behavior))
            {
                if (
                  Get<ComponentSys>()->GetBehavior(it->GetComponentHandle(ComponentKind::Behavior)).type
                  == BHVRType::BEAR)
                {
                  auto& trans_other = Get<ComponentSys>()->GetTransform(it->GetComponentHandle(ComponentKind::Transform));
                  auto& phys_other = Get<ComponentSys>()->GetPhysics(it->GetComponentHandle(ComponentKind::Physics));

                  if (Get<PhysicsSys>()->HasCollision(trans_plyr, player_physics.primitive, trans_other, phys_other.primitive))
                  {
                    playerHP_.ChangeCurrentHPby(1);
                    took_damage = true;
                    i_time = PLAYER_I_TIME;
                    Trace::Log(DEBUG) << "player hp:" << playerHP_.GetCurrentHP() << std::endl;
                    if (playerHP_.is_dead())
                    {
                      AudioPlayer::Play_Event(AK::EVENTS::PLAYER_DEATH);
                      Get<SceneSys>()->SetNextScene(SceneE::MenuScene);
                      return;
                    }
                    AudioPlayer::Play_Event(AK::EVENTS::PLAYER_HURT);
                    break;
                }
              }
            }
        }
    }
  
  //testing code
  /*if (took_damage)
  {
    i_time -= dt;

    if (i_time <= 0.f)
    {
      took_damage = false;
    }
  }
  else if (Get<InputHandler>()->KeyPressed(SDLK_y))
  {
    playerHP_.ChangeCurrentHPby(1);
    Trace::Log(DEBUG) << "player hp:" << playerHP_.GetCurrentHP() << std::endl;
    took_damage = true;
    i_time = PLAYER_I_TIME;

    if (playerHP_.is_dead())
    {
      Get<SceneSys>()->SetNextScene(SceneE::MenuScene);
      return;
    }
  }*/

  if (nextstate != movementstate_)
  {
    ExitState(movementstate_);
    movementstate_ = nextstate;
    EnterState(nextstate);
  }

  if (on_ground) {
    // stop downward velocity
    if (new_vel.m128_f32[1] < 0)
    {
      new_vel.m128_f32[1] = 0;
    }

    // set y to ground level.
    // HACKY -- remove once real static physics works
    auto pos = player_physics.rigid_body.GetPosition();
    pos.y = HACKY_GROUND_Y_LEVEL;
    player_physics.rigid_body.SetPosition(pos);
  } else {
    // maintain y velocity
    //float y_vel = player_physics.rigid_body.GetLinearVelocity().m128_f32[1];
    //new_vel.m128_f32[1] = y_vel;
  }

  player_physics.rigid_body.SetLinearVelocity(new_vel);
}

void PlayerMovementControllerSys::LevelEnd() {}

void PlayerMovementControllerSys::Unload() {}

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

  float v_side = (right & !left) ? 1.0f : (left & !right) ? -1.0f : 0.0f;
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

  move_dir = DirectX::XMVector3Normalize(move_dir);

  return move_dir;
}

// just no-ops, but can handle animation stuff later
void PlayerMovementControllerSys::ExitState(PlayerMovementControllerSys::MoveState) {}
void PlayerMovementControllerSys::EnterState(PlayerMovementControllerSys::MoveState) {}

} // namespace Octane