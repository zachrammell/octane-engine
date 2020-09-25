#pragma once

namespace Octane
{

// forward declaration to avoid circular include
class Engine;

enum class SystemOrder : unsigned int;

class ISystem
{
public:
  explicit ISystem(class Engine* parent_engine) : engine_(*parent_engine) {};

  virtual ~ISystem() = default;

  virtual void Load() = 0;
  virtual void LevelStart() = 0;
  virtual void Update() = 0;
  virtual void LevelEnd() = 0;
  virtual void Unload() = 0;

  // Intentionally unimplemented in the interface. Still necessary.
  static SystemOrder GetOrder() = delete;

  // simple convenience function to access engine getsystem function
  template<class System>
  System* Get()
  {
    return engine_.GetSystem<System>();
  }

protected:
  Engine& engine_;
};

} // namespace Octane
