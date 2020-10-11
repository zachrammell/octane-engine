#pragma once

namespace Octane
{
enum class eConstraintsType
{
  VelocityConstraints,
  PositionConstraints
};

class Constraints
{
public:
  Constraints() = default;

  virtual ~Constraints() {}

  virtual void Release() = 0;
  virtual void Generate(float dt) = 0;
  virtual void Solve(float dt) = 0;
  virtual void Apply() = 0;

public:
  eConstraintsType type = eConstraintsType::VelocityConstraints;
};
} // namespace Octane
