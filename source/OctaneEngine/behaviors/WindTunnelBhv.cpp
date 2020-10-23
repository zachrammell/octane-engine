/******************************************************************************/
/*!
  \par        Project Octane
  \file       WindTunnelBhv.cpp
  \author     Lowell Novitch
  \date       2020/10/23
  \brief      behavior for wind tunnel

  Copyright � 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

// Main include
#include <OctaneEngine/behaviors/WindTunnelBhv.h>
#include <iostream>
namespace Octane
{

WindTunnelBHV::WindTunnelBHV() {}

void WindTunnelBHV::Initialize() 
{
  std::cout << "tunnel init" << std::endl;
}

void WindTunnelBHV::Update() 
{
  std::cout << "tunnel update" << std::endl;
}

void WindTunnelBHV::Shutdown()
{
  std::cout << "tunnel shutdown" << std::endl;
}

}
