#pragma once

#include <array>
#include <type_traits>

#include <OctaneEngine/Helper.h>
#include <OctaneEngine/ISystem.h>
#include <OctaneEngine/SystemOrder.h>

namespace Octane
{

class Engine
{
public:
  Engine();
  template<class System>
  void AddSystem(System* sys);
  void Update();

  void Quit();

  void ChangeScene();
  void RestartScene();

  bool ShouldQuit() const;

  ISystem* GetSystem(SystemOrder sys);

  template<class System>
  System* GetSystem()
  {
    static_assert(std::is_base_of_v<ISystem, System>);
    return dynamic_cast<System*>(systems_[to_integral(System::GetOrder())]);
  }

private:
  std::array<ISystem*, to_integral(SystemOrder::COUNT)> systems_;
  int scene_changing_, scene_restarting_, should_quit_;
};

template<class System>
void Engine::AddSystem(System* sys)
{
  static_assert(std::is_base_of_v<ISystem, System>);
  systems_[to_integral(System::GetOrder())] = sys;
}

} // namespace Octane