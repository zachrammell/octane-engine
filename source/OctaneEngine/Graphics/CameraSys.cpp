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
#include "CameraSys.h"
#include "OctaneEngine/Engine.h"
#include "RenderDX11.h"

namespace Octane
{

namespace dx = DirectX;

CameraSys::CameraSys(Engine* engine)
: model(),view(),/*proj(),*/
right(1.f, 0.f, 0.f, 0.f), up(0.f, 1.f, 0.f, 0.f),
forward(0.f, 0.f, 1.f,0.f), pos(0.f,0.f,0.f, 1.f),
isDirty(true)
{
  //TODO: query window info because right now its hardcoded
  //projection matrix
  RecalcProjMat();

  engine->AddSystem(this);
}

void CameraSys::Load() 
{
}

void CameraSys::LevelStart() 
{
  //TODO: need some way to restart camera 
  //back to original position in scene
}

void CameraSys::Update() 
{
  if (isDirty)
  {
    isDirty = false;
    //model to world matrix
    model = dx::XMFLOAT4X4A(right.x, up.x, forward.x, pos.x,
      right.y,up.y,forward.y,pos.y,
      right.z,up.z,forward.z,pos.z,
      right.w,up.w,forward.w,pos.w);
    //invert and store in world to view matrix
    dx::XMStoreFloat4x4A(&view, dx::XMMatrixInverse(nullptr, dx::XMLoadFloat4x4A(&model)));
  }
}

void CameraSys::LevelEnd() 
{
}

void CameraSys::Unload() 
{
}

SystemOrder CameraSys::GetOrder()
{
  return SystemOrder::CameraSys;
}

dx::XMFLOAT4X4A CameraSys::View()
{
  return view;
}

dx::XMFLOAT4X4A CameraSys::Proj()
{
  return proj;
}

dx::XMFLOAT3A CameraSys::Pos()
{
  return dx::XMFLOAT3A(pos.x,pos.y,pos.z);
}

float CameraSys::NearDist()
{
  return nearDist;
}

float CameraSys::FarDist()
{
  return farDist;
}

void CameraSys::Pos(const dx::XMFLOAT3A& position)
{
  pos = dx::XMFLOAT4A(position.x,position.y, position.z, 1.f);
  isDirty = true;
}

void CameraSys::LookAt(const DirectX::XMFLOAT3A& position) 
{
  dx::XMVECTOR target = dx::XMLoadFloat3A(&position);
  dx::XMVECTOR myPos = dx::XMLoadFloat4A(&pos);
  dx::XMVECTOR upDir = dx::XMLoadFloat4A(&up);
  dx::XMMATRIX lookAt = dx::XMMatrixLookAtLH(myPos, target, upDir);
  dx::XMStoreFloat4A(&right, lookAt.r[0]);
  dx::XMStoreFloat4A(&up, lookAt.r[1]);
  dx::XMStoreFloat4A(&forward, lookAt.r[2]);
}

void CameraSys::NearDist(float distance)
{
  nearDist = distance;
  RecalcProjMat();
}

void CameraSys::FarDist(float distance) 
{
  farDist = distance;
  RecalcProjMat();
}

void CameraSys::RecalcProjMat(float width, float height) 
{
  proj = dx::XMFLOAT4X4A(
    2 * nearDist / width, //row 1
    0.f,
    0.f,
    0.f,

    0.f, //row 2
    2 * nearDist / height,
    0.f,
    0.f,

    0.f, //row 3
    0.f,
    farDist / (farDist - nearDist),
    1.f,

    0.f, //row 4
    0.f,
    (-farDist * nearDist) / (farDist - nearDist),
    0.f);
}

} // namespace Octane