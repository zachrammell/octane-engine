/******************************************************************************/
/*!
  \par        Project Octane
  \file       TestScene.cpp
  \author     Lowell Novitch
  \date       2020/10/6
  \brief      test scene

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

// Main include
#include <OctaneEngine/Scenes/TestScene.h>
#include <OctaneEngine/InputHandler.h>
#include <iostream>
namespace Octane
{
TestScene::TestScene(SceneSys* parent) : IScene(parent) {}

void TestScene::Load() 
{

}
void TestScene::Start() 
{

}
void TestScene::Update(float dt) 
{
 // auto* input = Get<InputHandler>();

  //if (input->KeyReleased(SDLK_SPACE))
  //{
  //  std::cout << "hello" << std::endl;
 // }

}
void TestScene::End() 
{

}
void TestScene::Unload() 
{

}

}
