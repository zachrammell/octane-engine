/******************************************************************************/
/*!
  \par        Project Octane
  \file       SerializationTestScene.h
  \author     Zach Rammell
  \date       2020/10/21
  \brief      test scene for serializing stuff

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once

// Includes
#include <OctaneEngine/SceneSys.h>

namespace Octane
{

class SerializationTestScene final : public IScene
{
public:
  explicit SerializationTestScene(SceneSys* parent);

  virtual void Load();
  virtual void Start();
  virtual void End();
  virtual void Unload();

  virtual void Update(float dt);
  virtual SceneE GetEnum() const;

private:
};

} // namespace Octane
