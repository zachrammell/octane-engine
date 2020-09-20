#pragma once

#include <OctaneEngine/ISystem.h>

namespace Octane
{

class InputHandler : public ISystem
{
  // ISystem implementation
public:
  InputHandler(class Engine* engine);
  ~InputHandler() = default;

  void Load() override {};
  void LevelStart() override {};
  void Update() override;
  void LevelEnd() override {};
  void Unload() override {};

  SystemOrder GetOrder() override;

private:
  class Engine* engine_;
};

}