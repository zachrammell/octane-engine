#pragma once

#include <OctaneEngine/behaviors/IBehavior.h>

namespace Octane
{
class TemporaryBehavior : public IBehavior
{
public:
  explicit TemporaryBehavior(BehaviorSys* parent, ComponentHandle handle, float lifetime);

  virtual ~TemporaryBehavior() = default;

  void Initialize() override;
  void Update(float dt, EntityID id) override;
  void Shutdown() override;

  void SetLifetime(float life) { lifetime_ = life; }

private:
  float lifetime_;
};

} // namespace Octane
