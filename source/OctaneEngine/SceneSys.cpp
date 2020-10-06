/******************************************************************************/
/*!
  \par        Project Octane
  \file       SceneSys.cpp
  \author     Lowell Novitch
  \date       2020/10/6
  \brief      Manages the current scene

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

// Main include
#include <OctaneEngine/SceneSys.h>
#include <OctaneEngine/SystemOrder.h>

namespace Octane
{
SceneSys::SceneSys(Engine* engine) : ISystem(engine) {};

void SceneSys::LevelStart() 
{
	
}

void SceneSys::Update() 
{

}

void SceneSys::LevelEnd() 
{

}

SystemOrder SceneSys::GetOrder()
{
  return SystemOrder::Scene;
}

}
