#pragma once

namespace Octane
{
class Constraints
{
public:
  Constraints() = default;

  virtual ~Constraints() {}
  virtual void Release() = 0;
  virtual void Generate(float dt) = 0;
  virtual void Solve(float dt) = 0;
  virtual void Apply() = 0;
};
} // namespace Octane
