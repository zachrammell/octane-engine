#pragma once

#include <EASTL/array.h>
#include <EASTL/type_traits.h>

#include <OctaneEngine/Helper.h>
#include <OctaneEngine/ISystem.h>
#include <OctaneEngine/SystemOrder.h>

namespace Octane
{

class Engine
{
public:
  Engine();
  ~Engine();

  template<class System>
  void AddSystem(System* sys);
  void Update();

  void Quit();

  void ChangeScene();
  void RestartScene();

  bool ShouldQuit() const;

  ISystem* GetSystem(SystemOrder sys);

  template<class System>
  [[nodiscard]] System* GetSystem()
  {
    static_assert(eastl::is_base_of_v<ISystem, System>);
    return static_cast<System*>(systems_[to_integral(System::GetOrder())]);
  }

private:
  eastl::array<ISystem*, to_integral(SystemOrder::COUNT)> systems_;
  int scene_changing_, scene_restarting_, should_quit_;
};

void CreateEngine();
void DeleteEngine();
Engine* GetEngine();

template<class System>
void Engine::AddSystem(System* sys)
{
  static_assert(eastl::is_base_of_v<ISystem, System>);
  systems_[to_integral(System::GetOrder())] = sys;
}

} // namespace Octane