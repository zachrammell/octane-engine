#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <iostream>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <EASTL/vector.h>

// EASTL expects user-defined new[] operators that it will use for memory allocation.
// TODO: make these return already-allocated memory from our own memory allocator.
void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
  return new uint8_t[size];
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
  return new uint8_t[size];
}

int main(int argc, char* argv[]) noexcept
{
  SDL_Window*  window  = nullptr;
  SDL_Surface* surface = nullptr;
  // must be the first thing in main, for SDL2 initialization
  SDL_SetMainReady();
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
  {
    std::cout << "SDL could not initialize! SDL_Error:" << SDL_GetError() << "\n";
  }
  else
  {
    int window_width  = 640;
    int window_height = 480;
    //Create window
    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
      std::cout << "Window could not be created! SDL_Error:" << SDL_GetError() << "\n";
    }
    else
    {
      //Get window surface
      surface = SDL_GetWindowSurface(window);
      //Fill the surface white
      SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));
      //Update the surface
      SDL_UpdateWindowSurface(window);
      //Wait 2 seconds
      SDL_Delay(2000);
    }
  }
  std::cout << "[== Project Octane ==]\n";
  eastl::vector<int> some_ints{0, 1, 42, 69, 420, 666};
  for (int i = 0; i < 1000; ++i)
  {
    some_ints.push_back(some_ints[i]);
  }
  // the EASTL does not throw exceptions - this will trigger a breakpoint but it will not crash the program
  //int garbage = some_ints.at(7);
  //std::cout << "Garbage, for your viewing pleasure: " << garbage << "\n";
  std::cout << "Some numbers: ";
  for (int element : some_ints)
  {
    std::cout << element << " ";
  }
  std::cout << "\n";
  SDL_DestroyWindow(window);
  SDL_Quit();
  _CrtDumpMemoryLeaks();
  return 0;
}
