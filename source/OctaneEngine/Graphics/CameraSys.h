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

#include <DirectXMath.h>
#include <OctaneEngine/Graphics/Camera.h>
#include <OctaneEngine/ISystem.h>

namespace Octane
{
//TODO: add ability to detect window resizing events
class CameraSys : public ISystem
{
public:
  //ISystem class functions
  explicit CameraSys(Engine* engine);
  void Load() override {}
  void LevelStart() override;
  void Update() override;
  void LevelEnd() override;
  void Unload() override {}
  static SystemOrder GetOrder();

  //CameraSys specific functions

  //getters
  FPSCamera& GetFPSCamera() { return fps_camera_; }
  DirectX::XMMATRIX GetProjectionMatrix();
  DirectX::XMMATRIX GetViewMatrix();

  float GetFOV() const { return fov_; }
  float GetNearDistance() const { return near_; }
  float GetFarDistance() const { return far_; }

  void SetFOV(float fov) { fov_ = fov; }
  void SetNearDistance(float near) { near_ = near; }
  void SetFarDistance(float far) { far_ = far; }

private:
  FPSCamera fps_camera_;
  DirectX::XMFLOAT4X4 proj_;
  float fov_, near_, far_;
  bool projection_dirty_;
};
} // namespace Octane