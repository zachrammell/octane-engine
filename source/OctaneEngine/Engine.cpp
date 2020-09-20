#include <OctaneEngine/Engine.h>

namespace Octane
{

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
}

void Engine::AddSystem(ISystem* sys)
{
  systems_[to_integral(sys->GetOrder())] = sys;
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
