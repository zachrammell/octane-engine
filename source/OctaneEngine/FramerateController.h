#pragma once

#include <cstdint>

#include <OctaneEngine/ISystem.h>

namespace Octane
{

class FramerateController : public ISystem
{
  // ISystem implementation
public:
  FramerateController()  = default;
  ~FramerateController() = default;

  void Load() override {};
  void LevelStart() override;
  void Update() override;
  void LevelEnd() override {};
  void Unload() override {};

  static SystemOrder GetOrder();

  // FramerateController API
public:
  float GetDeltaTime();

private:
  uint64_t current_time_;
  float delta_time_;
};

} // namespace Octane
