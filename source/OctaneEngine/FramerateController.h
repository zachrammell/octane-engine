#pragma once

#include <cstdint>

#include <OctaneEngine/ISystem.h>

namespace Octane
{

class FramerateController : public ISystem
{
  // ISystem implementation
public:
  FramerateController(Engine* Engine);
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

  // pausing will set deltatime to 0
  void Pause();
  void Unpause();
  bool isPaused() const;

  // returns the actual delta time even if game is paused
  float GetDeltaTimeIgnoringPause();

private:
  uint64_t current_time_;
  float delta_time_;
  bool paused_;
};

} // namespace Octane
