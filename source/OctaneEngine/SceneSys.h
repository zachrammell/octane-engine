/******************************************************************************/
/*!
  \par        Project Octane
  \file       SceneSys.h
  \author     Lowell Novitch
  \date       2020/10/6
  \brief      Manages the current scene

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once

// Includes
#include <OctaneEngine/ISystem.h>
#include <OctaneEngine/IScene.h>
namespace Octane
{

class SceneSys final : public ISystem
{
  // ISystem implementation
public:
  explicit SceneSys(Engine* engine);
  ~SceneSys() = default;

  void Load() override;
  void LevelStart() override;
  void Update() override;
  void LevelEnd() override;
  void Unload() override;

  static SystemOrder GetOrder();

  //scene manager
  void SetNextScene() {};

private:
  IScene* current_scene_ = nullptr;
};
}
