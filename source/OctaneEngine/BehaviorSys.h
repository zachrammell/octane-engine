/******************************************************************************/
/*!
  \par        Project Octane
  \file       BehaviorSys.h
  \author     Lowell Novitch
  \date       2020/10/6
  \brief      Manages and updates the behavior component of entities

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once

// Includes
#include <OctaneEngine/ISystem.h>

namespace Octane
{
//forward declaration
class EntitySys;

class BehaviorSys final : public ISystem
{
  // ISystem implementation
public:
  explicit BehaviorSys(Engine* engine);
  ~BehaviorSys() = default;

  void Load() override;
  void LevelStart() override;
  void Update() override;
  void LevelEnd() override;
  void Unload() override;

  static SystemOrder GetOrder();



private:
  EntitySys* entsys_;
};

}
