#pragma once

namespace Octane
{

enum class SystemOrder : unsigned int
{
  FramerateController,
  InputHandler,
  CameraSys,
  WindowManager,
  World,
  Scene,
  Entity,
  Component,

  RenderSys,
  // must be last, do not add anything after it.
  COUNT
};

}