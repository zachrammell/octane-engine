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
  SupportPoint(
    const DirectX::XMFLOAT3& global,
    const DirectX::XMFLOAT3& local1,
    const DirectX::XMFLOAT3& local2,
    size_t idx = 0);
  SupportPoint(const SupportPoint& rhs);
  ~SupportPoint();

  SupportPoint& operator=(const SupportPoint& rhs);
  DirectX::XMFLOAT3& operator[](size_t i);
  DirectX::XMFLOAT3 const& operator[](size_t i) const;
  bool operator==(const SupportPoint& rhs);
  bool operator==(const SupportPoint& rhs) const;
  bool IsValid() const;

public:
  DirectX::XMFLOAT3 global;
  DirectX::XMFLOAT3 local_a;
  DirectX::XMFLOAT3 local_b;
  size_t index;
};
} // namespace Octane
