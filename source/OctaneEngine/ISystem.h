#pragma once

namespace Octane
{

enum class SystemOrder : unsigned int;

class ISystem
{
public:
  ISystem() = default;
  explicit ISystem(class Engine*) {};
  virtual ~ISystem() = default;

  virtual void Load()       = 0;
  virtual void LevelStart() = 0;
  virtual void Update()     = 0;
  virtual void LevelEnd()   = 0;
  virtual void Unload()     = 0;

  // Intentionally unimplemented in the interface. Still necessary.
  static SystemOrder GetOrder();
};

} // namespace Octane
