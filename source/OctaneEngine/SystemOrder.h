#pragma once

namespace Octane
{

enum class SystemOrder : unsigned int
{
  FramerateController,
  InputHandler,
  ImGuiSys,
  PlayerMovementControllerSys,
  CameraSys,
  WindowManager,
  World,
  Scene,
  Entity,
  Component,
  RenderSys,
  Audio,
  // must be last, do not add anything after it.
  COUNT
};

}