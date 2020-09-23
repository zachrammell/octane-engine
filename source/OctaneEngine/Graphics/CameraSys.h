/******************************************************************************/
/*!
  \par        Project Octane
  \file       CameraSys.h
  \author     Brayan Lopez
  \date       09/22/2020
  \brief      Declaration of Camera System which will hold camera data.

  Copyright � 2020 DigiPen, All rights reserved.
  */
/******************************************************************************/
#pragma once

#include "OctaneEngine/ISystem.h"
#include <DirectXMath.h>

namespace Octane
{
//TODO: add ability to detect window resizing events
class CameraSys : public ISystem
{
public:
  //Isystem class functions
  CameraSys(Engine* engine);
  void Load();
  void LevelStart();
  void Update();
  void LevelEnd();
  void Unload();
  static SystemOrder GetOrder();

  //CameraSys specific functions

  //getters
  DirectX::XMFLOAT4X4A View();
  DirectX::XMFLOAT4X4A Proj();
  DirectX::XMFLOAT3A Pos();
  float NearDist();
  float FarDist();
  //setters
  void Pos(const DirectX::XMFLOAT3A& position);
  void LookAt(const DirectX::XMFLOAT3A& position);
  void NearDist(float distance);
  void FarDist(float distance);

private:
  //Recalculates the projection matrix
  void RecalcProjMat(float width = 1280.f, float height = 720.f);
  //camera matrices
  DirectX::XMFLOAT4X4A model;
  DirectX::XMFLOAT4X4A view;
  DirectX::XMFLOAT4X4A proj;
  //camera basis vectors
  DirectX::XMFLOAT4A right;
  DirectX::XMFLOAT4A up;
  DirectX::XMFLOAT4A forward;
  //TODO: maybe make this a Transform ID
  //once Transform is made
  DirectX::XMFLOAT4A pos;

  //distance of near and far plane
  float nearDist = 1.f;
  float farDist = 1000.f;

  //to remove redundant calculations of matrices
  bool isDirty;
};
} // namespace Octane