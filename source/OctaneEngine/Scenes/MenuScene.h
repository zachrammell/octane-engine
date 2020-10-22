/******************************************************************************/
/*!
  \par        Project Octane
  \file       MenuScene.h
  \author     Lowell Novitch
  \date       2020/10/16
  \brief      menu scene

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

class MenuScene final : public IScene
{
public:
  explicit MenuScene(SceneSys* parent);

  virtual void Load();
  virtual void Start();
  virtual void End();
  virtual void Unload();

  virtual void Update(float dt);
  virtual SceneE GetEnum() const;

private:
  SceneE selected_scene_;
};
}
