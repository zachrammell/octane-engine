#include <OctaneEngine/PlayerMovementControllerSys.h>

#include "AudioPlayer.h"
#include "FramerateController.h"
#include <OctaneEngine/AudioPlayer.h>
#include <OctaneEngine/Engine.h>
#include <OctaneEngine/Graphics/CameraSys.h>
#include <OctaneEngine/Graphics/RenderSys.h>
#include <OctaneEngine/InputHandler.h>
#include <OctaneEngine/Physics/PhysicsSys.h>
#include <OctaneEngine/SceneSys.h>
#include <OctaneEngine/SystemOrder.h>
#include <OctaneEngine/Trace.h>
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
const SDL_KeyCode KEY_ABILITY1 = SDLK_e; //spawn tunnel
const SDL_KeyCode KEY_ABILITY2 = SDLK_g; //spawn homming  tunnel

// these should be serialized later
const float PLAYER_SPEED = 7.0f;
const float PLAYER_CROUCH_SPEED = 3.0f;
const float PLAYER_AIRSTRAFE_MAXSPEED = 12.0f;
const float PLAYER_AIRSTRAFE_ACCEL = 4.5f;
const float PLAYER_GRAVITY_ACCEL = 9.81f * 2.0f;
const float PLAYER_HOVER_ACCEL = PLAYER_GRAVITY_ACCEL / 2.0f;
const float PLAYER_I_TIME = 1.0f; // invuln after getting hit
const int PLAYER_MAX_HP = 5;

const float NORMAL_FOV = 70.0f;
const float AIM_FOV = 50.0f;

// vertical velocity when starting jump
const float PLAYER_JUMP_VEL = 9.0f;

const DirectX::XMVECTOR ZERO_VEC = {0, 0, 0, 0};

// should be removed once we have usable collision stuff
const float HACKY_GROUND_Y_LEVEL = 1.0f;

// temporary(?) walk timer
static float walk_timer = 0;

DirectX::XMVECTOR bt_to_dx_vec3(btVector3 const& in)
{
  DirectX::XMFLOAT3 f3(in.x(), in.y(), in.z());
  return DirectX::XMLoadFloat3(&f3);
}

btVector3 dx_to_bt_vec3(DirectX::XMVECTOR const& in)
{
  // x,y,z components
  return btVector3(in.m128_f32[0], in.m128_f32[1], in.m128_f32[2]);
}

// returns true if the entity will damage the player by colliding
bool isDamagingObject(GameEntity& colliding_entity)
{
  // for now only checks for bear behavior
  if (colliding_entity.HasComponent(ComponentKind::Behavior))
  {
    bool isBear = GetEngine()
                    ->GetSystem<ComponentSys>()
                    ->GetBehavior(colliding_entity.GetComponentHandle(ComponentKind::Behavior))
                    .type
                  == BHVRType::BEAR;
    return isBear;
  }
  return false;
}

} // namespace

void PlayerMovementControllerSys::Load()
{
  movementstate_ = MoveState::STAND;

  // Load Player Audio stuff
  AudioPlayer::Load_Player();
}

PlayerMovementControllerSys::PlayerMovementControllerSys(Engine* engine)
  : ISystem(engine),
    movementstate_(MoveState::STAND)
{
}

PlayerMovementControllerSys::~PlayerMovementControllerSys() {}

void PlayerMovementControllerSys::LevelStart()
{
  playerHP_.set_maxHP(PLAYER_MAX_HP);
  playerHP_.setCurrentHP(PLAYER_MAX_HP);
  is_invuln_ = false;
}

void PlayerMovementControllerSys::Update()
{
  const float dt = Get<FramerateController>()->GetDeltaTime();
  if (dt == 0)
  {
    return;
  }
  EntityID const player_id = Get<EntitySys>()->GetPlayerID();
  GameEntity* const player = Get<EntitySys>()->GetPlayer();
  if (!player)
  {
    return; // no player exists, bail out
  }

  const ComponentHandle phys_id = player->GetComponentHandle(ComponentKind::Physics);
  PhysicsComponent& player_physics = Get<ComponentSys>()->GetPhysics(phys_id);
  TransformComponent& player_trans
    = Get<ComponentSys>()->GetTransform(player->GetComponentHandle(ComponentKind::Transform));

  const bool jump_input = Get<InputHandler>()->KeyPressedOrHeld(KEY_JUMP);
  const bool crouch_input = Get<InputHandler>()->KeyPressedOrHeld(KEY_CROUCH);
  const DirectX::XMVECTOR move_dir = CalcPlayerMoveDir();

  const bool is_moving = !DirectX::XMVector3Equal(move_dir, ZERO_VEC);

  const float dist_from_ground = Get<PhysicsSys>()->GetDistFromGround(player_id);
  // add an epsilon just to be sure
  const bool on_ground = (dist_from_ground <= player_trans.scale.y * 0.5f + 0.0001);

  MoveState nextstate = movementstate_;

  btVector3 old_vel_bt = player_physics.rigid_body->getLinearVelocity();
  DirectX::XMVECTOR old_vel = bt_to_dx_vec3(old_vel_bt);
  DirectX::XMVECTOR new_vel = old_vel;

  switch (movementstate_)
  {
    // these two use the same logic for now
  case MoveState::CROUCH:
  case MoveState::CROUCHWALK:
    if (!on_ground)
    {
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
    if (!on_ground)
    {
      nextstate = MoveState::JUMP;
    }
    else if (jump_input)
    {
      AudioPlayer::Play_Event(AK::EVENTS::PLAYER_JUMP);
      new_vel = DirectX::XMVectorAdd(
        DirectX::XMVectorScale(move_dir, PLAYER_SPEED),
        DirectX::XMVECTOR {0, PLAYER_JUMP_VEL, 0, 0});
      nextstate = MoveState::JUMP;
    }
    else if (is_moving)
    {
      // Handle walking sound
      if (walk_timer >= 100.0f / PLAYER_SPEED)
      {
        AudioPlayer::Play_Event(AK::EVENTS::PLAYER_FOOTSTEP);
        walk_timer = 0;
      }
      else
      {
        ++walk_timer;
      }
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
      auto const new_acc = DirectX::XMVectorScale(move_dir, PLAYER_AIRSTRAFE_ACCEL);

      // make sure to add to new_vel here rather than assigning so we maintain air velocity (esp. y velocity)
      new_vel = DirectX::XMVectorAdd(new_vel, DirectX::XMVectorScale(new_acc, dt));

      // clamp new_vel to max airspeed
      float squarespeed = DirectX::XMVector3LengthSq(new_vel).m128_f32[0];
      if (squarespeed > PLAYER_AIRSTRAFE_MAXSPEED * PLAYER_AIRSTRAFE_MAXSPEED)
      {
        new_vel = DirectX::XMVectorScale(DirectX::XMVector3Normalize(new_vel), PLAYER_AIRSTRAFE_MAXSPEED);
      }

      // only hover if player is moving downwards and holding jump
      if (new_vel.m128_f32[1] < 0 && jump_input)
      {
        player_physics.SetGravity(-PLAYER_HOVER_ACCEL);
      }
      else
      {
        player_physics.SetGravity(-PLAYER_GRAVITY_ACCEL);
      }
    }
    break;
  }

  //damage stuff
  UpdateDamage();

  if (nextstate != movementstate_)
  {
    ExitState(movementstate_);
    movementstate_ = nextstate;
    EnterState(nextstate);
  }

  else
  {
    // not on ground, so we can be blown by wind
    DirectX::XMFLOAT3 wind_force = GetWindTunnelForce();
    wind_force.x *= dt;
    wind_force.y *= dt;
    wind_force.z *= dt;

    new_vel.m128_f32[0] += wind_force.x;
    new_vel.m128_f32[1] += wind_force.y;
    new_vel.m128_f32[2] += wind_force.z;

    // why not use the following instead of changing velocity?
    // player_physics.rigid_body->applyCentralForce(wind_force);
  }

  // activate the rigid body so it will definitely respond to setLinearVelocity
  // (otherwise it only updates physics if it's been acted on by another physics object recently)
  player_physics.rigid_body->setActivationState(ACTIVE_TAG);
  player_physics.rigid_body->setLinearVelocity(dx_to_bt_vec3(new_vel));
  UpdateLookDir();

  //player abilities
  if (Get<InputHandler>()->KeyReleased(KEY_ABILITY1))
  {
    // EntityID tunnel = Get<EntitySys>()->MakeEntity();

    GameEntity& obj102_entity = Get<EntitySys>()->GetEntity((Get<EntitySys>()->MakeEntity()));
    ComponentHandle trans_id = Get<ComponentSys>()->MakeTransform();
    obj102_entity.components[to_integral(ComponentKind::Transform)] = trans_id;
    TransformComponent& trans = Get<ComponentSys>()->GetTransform(trans_id);
    //spawn offset
    DirectX::XMVECTOR cam_dir = Get<CameraSys>()->GetFPSCamera().GetViewDirection();
    DirectX::XMVECTOR offset = DirectX::XMVectorScale(DirectX::XMVector3Normalize(cam_dir), 5.0f);
    DirectX::XMVECTOR playpos = DirectX::XMLoadFloat3(&player_trans.pos);
    DirectX::XMStoreFloat3(
      &trans.pos,
      DirectX::XMVectorAdd(
        offset,
        playpos)); //add the offest and the player pos and set it to the transfor of the new windtunnel
    trans.pos.y = HACKY_GROUND_Y_LEVEL; //make sure it is on the ground
    trans.scale = {2.0f, 2.0f, 2.0f};
    //trans.rotation = {};

    ComponentHandle render_comp_id = Get<ComponentSys>()->MakeRender();
    obj102_entity.components[to_integral(ComponentKind::Render)] = render_comp_id;
    RenderComponent& render_comp = Get<ComponentSys>()->GetRender(render_comp_id);
    render_comp.material.diffuse = Colors::red;
    render_comp.mesh_type = Mesh_Key {"Cube"};
    render_comp.render_type = RenderType::Wireframe;

    ComponentHandle physics_comp_id = Get<ComponentSys>()->MakePhysicsBox(trans, {4.0f, 4.0f, 4.0f}, 0.0f, true);
    obj102_entity.components[to_integral(ComponentKind::Physics)] = physics_comp_id;
    PhysicsComponent& physics_comp = Get<ComponentSys>()->GetPhysics(physics_comp_id);

    obj102_entity.components[to_integral(ComponentKind::Behavior)]
      = Get<ComponentSys>()->MakeBehavior(BHVRType::ABILITYTUNNEL);
  }
  if (Get<InputHandler>()->KeyReleased(KEY_ABILITY2))
  {
    // EntityID tunnel = Get<EntitySys>()->MakeEntity();

    GameEntity& obj102_entity = Get<EntitySys>()->GetEntity((Get<EntitySys>()->MakeEntity()));
    ComponentHandle trans_id = Get<ComponentSys>()->MakeTransform();
    obj102_entity.components[to_integral(ComponentKind::Transform)] = trans_id;
    TransformComponent& trans = Get<ComponentSys>()->GetTransform(trans_id);
    //spawn offset
    DirectX::XMVECTOR cam_dir = Get<CameraSys>()->GetFPSCamera().GetViewDirection();
    DirectX::XMVECTOR offset = DirectX::XMVectorScale(DirectX::XMVector3Normalize(cam_dir), 5.0f);
    DirectX::XMVECTOR playpos = DirectX::XMLoadFloat3(&player_trans.pos);
    DirectX::XMStoreFloat3(
      &trans.pos,
      DirectX::XMVectorAdd(
        offset,
        playpos));      //add the offest and the player pos and set it to the transfor of the new windtunnel
    trans.pos.y = 1.0f; //make sure it is on the ground
    trans.scale = {2.0f, 2.0f, 2.0f};
    //trans.rotation = {};

    ComponentHandle render_comp_id = Get<ComponentSys>()->MakeRender();
    obj102_entity.components[to_integral(ComponentKind::Render)] = render_comp_id;
    RenderComponent& render_comp = Get<ComponentSys>()->GetRender(render_comp_id);
    render_comp.material.diffuse = Colors::blue;
    render_comp.mesh_type = Mesh_Key {"Cube"};
    render_comp.render_type = RenderType::Wireframe;

    ComponentHandle physics_comp_id = Get<ComponentSys>()->MakePhysicsBox(trans, {4.0f, 4.0f, 4.0f}, 0.0f, true);
    obj102_entity.components[to_integral(ComponentKind::Physics)] = physics_comp_id;
    PhysicsComponent& physics_comp = Get<ComponentSys>()->GetPhysics(physics_comp_id);

    obj102_entity.components[to_integral(ComponentKind::Behavior)]
      = Get<ComponentSys>()->MakeBehavior(BHVRType::ABILITYHOMMING);
  }

  AudioPlayer::Update_Player(player_trans.pos);
}

void PlayerMovementControllerSys::LevelEnd() {}

void PlayerMovementControllerSys::Unload()
{
  // Unload player stuff
  AudioPlayer::Unload_Player();
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

void PlayerMovementControllerSys::UpdateDamage()
{
  const float dt = Get<FramerateController>()->GetDeltaTime();
  if (is_invuln_)
  {
    remaining_invuln_time_ -= dt;

    if (remaining_invuln_time_ <= 0.f)
    {
      is_invuln_ = false;
    }
  }
  else if (CheckForEnemyCollision())
  {
    playerHP_.ChangeCurrentHPby(1);
    is_invuln_ = true;
    remaining_invuln_time_ = PLAYER_I_TIME;
    Trace::Log(DEBUG) << "player hp:" << playerHP_.GetCurrentHP() << std::endl;
    if (playerHP_.is_dead())
    {
      AudioPlayer::Play_Event(AK::EVENTS::PLAYER_DEATH);
      Get<SceneSys>()->SetNextScene(SceneE::MenuScene);
    }
    else
    {
      AudioPlayer::Play_Event(AK::EVENTS::PLAYER_HURT);
    }
  }
}

// returns true if the player has collided with at least one enemy this frame
bool PlayerMovementControllerSys::CheckForEnemyCollision()
{
  GameEntity* player = Get<EntitySys>()->GetPlayer();

  auto range = Get<PhysicsSys>()->GetCollisions(player->GetComponentHandle(ComponentKind::Physics));
  for (auto it = range.first; it != range.second; ++it)
  {
    PhysicsComponent& rhs = Get<ComponentSys>()->GetPhysics(it->second);
    assert(rhs.parent_entity != INVALID_ENTITY);
    GameEntity& rhs_ent = Get<EntitySys>()->GetEntity(rhs.parent_entity);

    if (isDamagingObject(rhs_ent))
    {
#ifdef _DEBUG
      {
        DirectX::XMFLOAT3 lhspos
          = Get<ComponentSys>()->GetTransform(player->GetComponentHandle(ComponentKind::Transform)).pos;
        DirectX::XMFLOAT3 rhspos
          = Get<ComponentSys>()->GetTransform(rhs_ent.GetComponentHandle(ComponentKind::Transform)).pos;

        Trace::Log(DEBUG) << "Player colliding with enemy, player transform: (" << lhspos.x << ", " << lhspos.y << ", "
                          << lhspos.z << ") enemy transform: (" << rhspos.x << ", " << rhspos.y << ", " << rhspos.z
                          << ") \n";
      }
#endif

      return true;
    }
  }
  return false; // no collisions
}
void PlayerMovementControllerSys::UpdateLookDir()
{
  auto* input = Get<InputHandler>();
  auto& camera = Get<CameraSys>()->GetFPSCamera();

  DirectX::XMINT2 mouse_vel = input->GetMouseMovement();
  camera.RotatePitchRelative(-mouse_vel.y * MOUSE_SENS);
  camera.RotateYawRelative(mouse_vel.x * MOUSE_SENS);

  if (input->MouseButtonPressed(InputHandler::MouseButton::RIGHT))
  {
    Get<CameraSys>()->SetFOV(AIM_FOV);
  }
  if (input->MouseButtonReleased(InputHandler::MouseButton::RIGHT))
  {
    Get<CameraSys>()->SetFOV(NORMAL_FOV);
  }
}

// gets the force from the first colliding wind tunnel
DirectX::XMFLOAT3 PlayerMovementControllerSys::GetWindTunnelForce()
{
  GameEntity* player = Get<EntitySys>()->GetPlayer();

  auto range = Get<PhysicsSys>()->GetCollisions(player->GetComponentHandle(ComponentKind::Physics));
  for (auto it = range.first; it != range.second; ++it)
  {
    PhysicsComponent& rhs = Get<ComponentSys>()->GetPhysics(it->second);
    assert(rhs.parent_entity != INVALID_ENTITY);
    GameEntity& rhs_ent = Get<EntitySys>()->GetEntity(rhs.parent_entity);

    if (rhs_ent.HasComponent(ComponentKind::Behavior))
    {
      auto bhvtype = Get<ComponentSys>()->GetBehavior(rhs_ent.GetComponentHandle(ComponentKind::Behavior)).type;
      if (bhvtype == BHVRType::WINDTUNNEL || bhvtype == BHVRType::ABILITYTUNNEL)
      {
        return Get<ComponentSys>()->GetBehavior(rhs_ent.GetComponentHandle(ComponentKind::Behavior)).force;
      }
    }
  }

  return {0, 0, 0};
}
} // namespace Octane
