﻿/******************************************************************************/
/*!
  \par        Project Octane
  \file       WindowManager.h
  \author     Zach Rammell
  \date       2020/09/25
  \brief      <WHAT DOES IT DO>

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once

// Includes
#include <OctaneEngine/ISystem.h>

#include <SDL.h>

namespace Octane
{

class WindowManager : public ISystem
{
public:
  WindowManager(class Engine* parent, char const* title, int width, int height);
  ~WindowManager();

  virtual void Load() {};
  virtual void LevelStart() {};
  virtual void Update() {};
  virtual void LevelEnd() {};
  virtual void Unload() {};

  static SystemOrder GetOrder();

  SDL_Window* GetHandle();

  // TODO: resizing callbacks
  int GetWidth();
  int GetHeight();

  float GetAspectRatio();

private:
  SDL_Window* window_;
};

}