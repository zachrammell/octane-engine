#pragma once

#include <directxmath.h>

namespace Octane
{
    inline DirectX::XMFLOAT3 HadamardProduct(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
    {
        return DirectX::XMFLOAT3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
    }

    inline DirectX::XMVECTOR HadamardProduct(const DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs)
    {
        DirectX::XMFLOAT3 float3(lhs.m128_f32[0] * rhs.m128_f32[0], lhs.m128_f32[1] * rhs.m128_f32[1], lhs.m128_f32[2] * rhs.m128_f32[2]);
        return XMLoadFloat3(&float3);
    }

    inline DirectX::XMVECTOR HadamardProduct(const DirectX::XMVECTOR& lhs, const DirectX::XMFLOAT3& rhs)
    {
        DirectX::XMFLOAT3 float3(lhs.m128_f32[ 0 ] * rhs.x, lhs.m128_f32[ 1 ] * rhs.y, lhs.m128_f32[ 2 ] * rhs.z);
        return XMLoadFloat3(&float3);
    }

    inline DirectX::XMFLOAT3X3 OuterProduct(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
    {
        return DirectX::XMFLOAT3X3(
                                   lhs.x * rhs.x, lhs.x * rhs.y, lhs.x * rhs.z,
                                   lhs.y * rhs.x, lhs.y * rhs.y, lhs.y * rhs.z,
                                   lhs.z * rhs.x, lhs.z * rhs.y, lhs.z * rhs.z
                                  );
    }
}
