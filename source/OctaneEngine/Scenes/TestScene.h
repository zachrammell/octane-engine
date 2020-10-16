/******************************************************************************/
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
//forward ref
class InputHandler;

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
  InputHandler& inhand_;

  bool esc_menu = false;
  bool demo_window_open = false;
};
}
