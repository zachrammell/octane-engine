#pragma once

namespace Octane
{

enum class SystemOrder : uint32_t
{
  FramerateController,
  InputHandler,
  ImGuiSys,
  PlayerMovementControllerSys,
  CameraSys,
  WindowManager,
  PhysicsSys,
  SceneSys,
  BehaviorSys,
  EntitySys,
  ComponentSys,
  RenderSys,
  MeshSys,
  Audio,
  // must be last, do not add anything after it.
  COUNT
};

}
