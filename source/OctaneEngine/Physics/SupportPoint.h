#pragma once

#include <OctaneEngine/Math.h>

namespace Octane
{
class SupportPoint
{
public:
  SupportPoint();
  SupportPoint(
    const DirectX::XMVECTOR& global,
    const DirectX::XMVECTOR& local1,
    const DirectX::XMVECTOR& local2,
    size_t idx = 0);
  SupportPoint(const SupportPoint& rhs);
  ~SupportPoint();

  SupportPoint& operator=(const SupportPoint& rhs);
  DirectX::XMVECTOR& operator[](size_t index);
  DirectX::XMVECTOR operator[](size_t index) const;
  bool operator==(const SupportPoint& rhs);
  bool operator==(const SupportPoint& rhs) const;
  bool IsValid() const;

public:
  DirectX::XMVECTOR global;
  DirectX::XMVECTOR local_a;
  DirectX::XMVECTOR local_b;
  size_t index;
};
} // namespace Octane
