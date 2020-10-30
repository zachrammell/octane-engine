#include <OctaneEngine/Engine.h>

#include <iostream>
#include <cassert>

#include <SDL.h>

namespace Octane
{

namespace {
// systems in this array will be shutdown first, in the order they appear
// afterwards, all remaining systems will be shutdown
const SystemOrder PRIORITY_SHUTDOWN_ORDER[] = {
  SystemOrder::WindowManager,
  SystemOrder::Audio
};
}

Engine::Engine()
    // Start with all systems missing
  : systems_ {nullptr},
    // 
    scene_changing_ {true},
    // 
    scene_restarting_ {false},
    //
    should_quit_{false}
{
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
  {
    std::clog << "SDL could not initialize! SDL_Error:" << SDL_GetError() << "\n";
    assert(!"Could not initialize SDL.");
  }
}

Engine::~Engine()
{
  // shutdown some systems in priority order
  for (SystemOrder idx : PRIORITY_SHUTDOWN_ORDER) {
    ISystem*& system = systems_[to_integral(idx)];
    if (system) {
      delete system;
      system = nullptr;
    }
  }

  // shutdown the rest
  for (ISystem* system : systems_)
  {
    if (system)
    {
      delete system;
      system = nullptr;
    }
  }
  SDL_Quit();
}

void Engine::Update()
{
  if (scene_changing_ || scene_restarting_)
  {
    for (ISystem* system : systems_)
    {
      if (system)
      {
        system->LevelEnd();
      }
    }
  }
  if (scene_changing_)
  {
    for (ISystem* system : systems_)
    {
      if (system)
      {
        system->Unload();
      }
    }
    for (ISystem* system : systems_)
    {
      if (system)
      {
        system->Load();
      }
    }
  }
  if (scene_changing_ || scene_restarting_)
  {
    for (ISystem* system : systems_)
    {
      if (system)
      {
        system->LevelStart();
      }
    }
  }
  scene_changing_ = scene_restarting_ = false;

  for (ISystem* system : systems_)
  {
    // Only update the systems that are present
    if (system)
    {
      system->Update();
    }
  }
}

void Engine::Quit()
{
  should_quit_ = true;
}

void Engine::ChangeScene()
{
  scene_changing_ = true;
}

void Engine::RestartScene()
{
  scene_restarting_ = true;
}

bool Engine::ShouldQuit() const
{
  return should_quit_;
}

ISystem* Engine::GetSystem(SystemOrder sys)
{
  return systems_[to_integral(sys)];
}

} // namespace Octane
