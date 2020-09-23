#pragma once

namespace Octane
{

enum class SystemOrder : unsigned int
{
  FramerateController,
  InputHandler,
  CameraSys,
  // must be last, do not add anything after it.
  COUNT
};

}