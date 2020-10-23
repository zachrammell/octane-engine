/******************************************************************************/
/*!
  \par        Project Octane
  \file       IBehavior.h
  \author     Lowell Novitch
  \date       2020/10/23
  \brief      behavior interface class

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once

// Includes

namespace Octane
{

class IBehavior
{
public:
  explicit IBehavior() {};

  virtual ~IBehavior() = default;

  virtual void Initialize() = 0;
  virtual void Update() = 0;
  virtual void Shutdown() = 0;

};

}
