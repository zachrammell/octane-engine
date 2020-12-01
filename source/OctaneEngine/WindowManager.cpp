/******************************************************************************/
/*!
  \par        Project Octane
  \file       WindowManager.cpp
  \author     <WHO WROTE THIS>
  \date       YYYY/MM/DD
  \brief      <WHAT DOES IT DO>

  Copyright � 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

// Main include
#include <OctaneEngine/WindowManager.h>

#include <cassert>
#include <iostream>

#include <OctaneEngine/Engine.h>
#include <OctaneEngine/Graphics/RenderSys.h>
#include <OctaneEngine/SystemOrder.h>

namespace Octane
{
WindowManager::WindowManager(Engine* parent, char const* title, int width, int height) : ISystem(parent)
{
  int targetwidth = width;
  int targetheight = height;

#ifndef _DEBUG
  SDL_DisplayMode mode;
  int err = SDL_GetDisplayMode(0, 0, &mode);
  if (!err)
  {
    targetwidth = mode.w;
    targetheight = mode.h;
  }
  else
  {
    std::clog << "SDL_GetDisplayMode failed: " << SDL_GetError() << "\n";
    std::clog << "Falling back to default width/height\n";

    // leave targetwidth and targetheight as the defaults set above
  }
#endif

  //Create window
  window_ = SDL_CreateWindow(
    title,
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    targetwidth,
    targetheight,
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
void WindowManager::SetFullscreen(bool fullscreen)
{
  // SDL_WINDOW_FULLSCREEN_DESKTOP = borderless, SDL_WINDOW_FULLSCREN = true fullscreen, 0 = windowed
  if (fullscreen)
  {
    SDL_SetWindowFullscreen(window_, SDL_WINDOW_FULLSCREEN_DESKTOP);
  }
  else
  {
    SDL_SetWindowFullscreen(window_, 0);
  }
  Get<RenderSys>()->OnResize();
}
bool WindowManager::IsFullscreen() const
{
  Uint32 flags = SDL_GetWindowFlags(window_);

  // check for either of these two flags, true fullscreen or borderless
  bool isfullscreen = flags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP);
  return isfullscreen;
}

} // namespace Octane
