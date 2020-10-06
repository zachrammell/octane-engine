/******************************************************************************/
/*!
  \par        Project Octane
  \file       IScene.h
  \author     Lowell Novitch
  \date       2020/10/06
  \brief      is the class definition for a scene object

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once

// Includes

namespace Octane
{
	class IScene
	{
        public:
			//constructor
          IScene() {};

		  //destructor
		  virtual ~IScene() = default;

		  virtual void Load() = 0;
		  virtual void Start() = 0;
		  virtual void End() = 0;
		  virtual void Unload() = 0;

		  virtual void Update(float dt) = 0;
	};
}
