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
#include <string>

namespace Octane
{
class SceneSys;

	class IScene
	{
        public:
			//constructor
          explicit IScene(class SceneSys* parent) : parent_manager_ {*parent} {};

		  //destructor
		  virtual ~IScene() = default;

		  virtual void Load() = 0;
		  virtual void Start() = 0;
		  virtual void End() = 0;
		  virtual void Unload() = 0;

		  virtual void Update(float dt) = 0;

		   // simple convenience function to access engine getsystem function
           //template<class System>
          // System* Get()
          // {
          //   return parent_manager_.Get<System>();
         //  }
                  virtual std::string GetName() const { return ""; }
                    
		private:
          SceneSys& parent_manager_;
	};
}
