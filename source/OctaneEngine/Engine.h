#pragma once

#include <array>

#include <OctaneEngine/ISystem.h>
#include <OctaneEngine/SystemOrder.h>
#include <OctaneEngine/Helper.h>

namespace Octane
{

class Engine
{
public:
  Engine();
  void AddSystem(ISystem* sys);
  void Update();

  void ChangeScene();
  void ReloadScene();

  ISystem* GetSystem(SystemOrder sys);

private:
  std::array<ISystem*, to_integral(SystemOrder::COUNT)> systems_;
  int scene_changing_, scene_restarting_;
};

} // namespace Octane