/******************************************************************************/
/*!
  \par        Project Octane
  \file       CameraSys.cpp
  \author     Brayan Lopez
  \date       09/22/2020
  \brief      Implementation of Camera System which will hold camera data.

  Copyright � 2020 DigiPen, All rights reserved.
  */
/******************************************************************************/

#include <OctaneEngine/Engine.h>
#include <OctaneEngine/Entity.h>
#include <OctaneEngine/EntitySys.h>
#include <OctaneEngine/Graphics/CameraSys.h>
#include <OctaneEngine/Graphics/GraphicsDeviceDX11.h>
#include <OctaneEngine/SystemOrder.h>
#include <OctaneEngine/WindowManager.h>

namespace Octane
{

namespace dx = DirectX;

CameraSys::CameraSys(Engine* engine)
  : ISystem(engine),
    fps_camera_ {DirectX::XMVectorSet(0.0f, 1.0f, 10.0f, 1.0f)},
    proj_ {},
    fov_ {70.0f},
    near_ {0.05f},
    far_ {1000.0f},
    projection_dirty_ {true}
{
}

void CameraSys::LevelStart()
{
  //TODO: need some way to restart camera
  //back to original position in scene
  fps_camera_.SetPosition(DirectX::XMVectorSet(0.0f, 1.0f, 10.0f, 1.0f));
  fps_camera_.RotatePitchAbsolute(0);
  fps_camera_.RotateYawAbsolute(0);
}

void CameraSys::Update() {
  // set camera position to player, if player exists
  GameEntity* player = Get<EntitySys>()->GetPlayer();
  if (player) {
    DirectX::XMFLOAT3 pos = Get<ComponentSys>()->GetTransform(player->GetComponentHandle(ComponentKind::Transform)).pos;
    GetFPSCamera().SetPosition(DirectX::XMLoadFloat3(&pos));
  }
}

void CameraSys::LevelEnd() {}

SystemOrder CameraSys::GetOrder()
{
  return SystemOrder::CameraSys;
}

dx::XMMATRIX CameraSys::GetProjectionMatrix()
{
  if (projection_dirty_)
  {
    dx::XMMATRIX new_proj = DirectX::XMMatrixPerspectiveFovRH(
      DirectX::XMConvertToRadians(fov_),
      Get<WindowManager>()->GetAspectRatio(),
      near_,
      far_);
    dx::XMStoreFloat4x4(&proj_, new_proj);
    return new_proj;
  }
  return dx::XMLoadFloat4x4(&proj_);
}

DirectX::XMMATRIX CameraSys::GetViewMatrix()
{
  return fps_camera_.GetViewMatrix();
}

} // namespace Octane