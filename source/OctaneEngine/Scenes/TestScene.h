﻿/******************************************************************************/
/*!
  \par        Project Octane
  \file       TestScene.h
  \author     Lowell Novitch
  \date       2020/10/6
  \brief      test scene

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once

// Includes
#include <OctaneEngine/SceneSys.h>

namespace Octane
{

class TestScene final : public IScene
{
public:
  explicit TestScene(SceneSys * parent);

  virtual void Load();
  virtual void Start();
  virtual void End();
  virtual void Unload();

  virtual void Update(float dt);
  virtual SceneE GetEnum() const;

private:
  bool esc_menu = false;
  bool demo_window_open = false;
  bool zoom_button = false;
  bool continue_shoot = false;
  float fov = 0.0f;
  int mouse_sens = 30;
};
}
