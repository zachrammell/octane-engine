/******************************************************************************/
/*!
  \par        Project Octane
  \file       ImGuiSys.h
  \author     Zach Rammell
  \date       2020/10/15
  \brief      Initializes and updates the ImGui library

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once

// Includes
#include <OctaneEngine/ISystem.h>

namespace Octane
{

class ImGuiSys : public ISystem
{
public:

  explicit ImGuiSys(class Engine* parent_engine);

  ~ImGuiSys() = default;

  virtual void Load() {};
  virtual void LevelStart() {};
  virtual void Update();
  virtual void LevelEnd() {};
  virtual void Unload() {};

  // Intentionally unimplemented in the interface. Still necessary.
  static SystemOrder GetOrder();

  static void StyleVisualStudio();
  // to be called by RenderSys
  void Render();
};

}
