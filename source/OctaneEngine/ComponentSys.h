#pragma once

#include <EASTL/vector.h>
#include "ISystem.h"
#include "SystemOrder.h"

namespace Octane {

enum class ComponentKind
{
  transform = 0,
  physics,
  render,
  COUNT,
};

typedef int ComponentHandle;
const ComponentHandle INVALID_COMPONENT = -1;

class ComponentSys : public ISystem {
public:
  explicit ComponentSys(class Engine* parent_engine);

  void Load() override;
  void LevelStart() override;
  void Update() override;
  void LevelEnd() override;
  void Unload() override;

  static SystemOrder GetOrder();

  // not sure how to implement type-specific component getting yet..

private:
  static const SystemOrder ORDER = SystemOrder::Component;
};

}