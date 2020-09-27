/******************************************************************************/
/*!
  \par        Project Octane
  \file       WindowManager.cpp
  \author     <WHO WROTE THIS>
  \date       YYYY/MM/DD
  \brief      <WHAT DOES IT DO>

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

// Main include
#include <OctaneEngine/WindowManager.h>

#include <cassert>
#include <iostream>

#include <OctaneEngine/SystemOrder.h>

namespace Octane
{
WindowManager::WindowManager(Engine* parent, char const* title, int width, int height) : ISystem(parent)
{
  // TODO: parameterize
  int window_width = 1280;
  int window_height = 720;
  //Create window
  window_ = SDL_CreateWindow(
    "Project Octane",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    window_width,
    window_height,
    SDL_WINDOW_SHOWN);
  if (window_ == nullptr)
  {
    std::clog << "Window could not be created! SDL_Error:" << SDL_GetError() << "\n";
    assert(!"Could not open SDL window.");
  }
}

WindowManager::~WindowManager()
{
  SDL_DestroyWindow(window_);
}

SystemOrder WindowManager::GetOrder()
{
  return SystemOrder::WindowManager;
}

SDL_Window* WindowManager::GetHandle()
{
  return window_;
}

int WindowManager::GetWidth()
{
  // TODO: store this? need to make sure it is never stale
  int width;
  SDL_GetWindowSize(window_, &width, nullptr);
  return width;
}

int WindowManager::GetHeight()
{
  int height;
  SDL_GetWindowSize(window_, nullptr, &height);
  return height;
}

float WindowManager::GetAspectRatio()
{
  return static_cast<float>(GetWidth()) / static_cast<float>(GetHeight());
}

} // namespace Octane
