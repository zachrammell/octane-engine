#pragma once

#include <EASTL/type_traits.h>

namespace Octane
{

// forward declaration to avoid circular include
class Engine;

enum class SystemOrder : unsigned int;

class ISystem
{
public:
  explicit ISystem(class Engine* parent_engine) : engine_ {*parent_engine} {};

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
    static_assert(eastl::is_base_of_v<ISystem, System>);
    return engine_.GetSystem<System>();
  }

  // specialization on Engine to allow accessing it with the same syntax
  template <>
  Engine* Get()
  {
    return &engine_;
  }

private:
  Engine& engine_;
};

} // namespace Octane
