#pragma once

#include <DirectXMath.h>
#include <limits>

namespace Octane
{
namespace Math
{
const float PI = 3.14159265358979323846264f;
const float HALF_PI = PI * 0.5f;
const float QUARTER_PI = PI * 0.25f;
const float TWO_PI = PI * 2.f;
const float PI_OVER_2 = HALF_PI;
const float PI_OVER_3 = PI / 3;
const float PI_OVER_4 = QUARTER_PI;
const float PI_OVER_6 = PI / 6;
const float RADIAN = PI / 180.0f;

const float EPSILON = 0.00001f;
const float EPSILON_SQUARED = EPSILON * EPSILON;
const float EPSILON_BIAS = 1.00001f;

const float REAL_MAX = 3.402823466e+38F;
const float REAL_MIN = 1.175494351e-38F;

const float REAL_POSITIVE_MAX = REAL_MAX;  //go to +max
const float REAL_POSITIVE_MIN = REAL_MIN;  //near to 0
const float REAL_NEGATIVE_MAX = -REAL_MAX; //go to -max
const float REAL_NEGATIVE_MIN = -REAL_MIN; //near to 0

const float ROOT_TWO = 1.41421356237309504880168f;
const float ROOT_THREE = 1.73205080756887729352744f;
const float ROOT_FIVE = 2.23606797749978969640917f;
const float ROOT_TEN = 3.16227766016837933199889f;
const float CUBE_ROOT_TWO = 1.25992104989487316476721f;
const float CUBE_ROOT_THREE = 1.25992104989487316476721f;
const float FORTH_ROOT_TWO = 1.18920711500272106671749f;

const float LN_TWO = 0.69314718055994530941723f;
const float LN_THREE = 1.09861228866810969139524f;
const float LN_TEN = 2.30258509299404568401799f;

const float F_NAN = std::numeric_limits<float>::quiet_NaN();
const float F_INF = std::numeric_limits<float>::infinity();

inline bool IsEqual(float a, float b)
{
  return (fabsf(a - b) < Math::EPSILON);
}

inline bool IsNotEqual(float a, float b)
{
  return !(fabsf(a - b) < Math::EPSILON);
}

inline bool IsZero(float a)
{
  return (fabsf(a) < Math::EPSILON);
}

inline bool IsValid(float a)
{
  return (!isnan(a)) && isfinite(a);
}

inline bool IsEqual(const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b)
{
  return (
    IsEqual(a.m128_f32[0], b.m128_f32[0]) && IsEqual(a.m128_f32[1], b.m128_f32[1])
    && IsEqual(a.m128_f32[2], b.m128_f32[2]) && IsEqual(a.m128_f32[3], b.m128_f32[3]));
}

inline bool IsEqualVector3(const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b)
{
  return (
    IsEqual(a.m128_f32[0], b.m128_f32[0]) && IsEqual(a.m128_f32[1], b.m128_f32[1])
    && IsEqual(a.m128_f32[2], b.m128_f32[2]));
}

inline bool IsNotEqual(const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b)
{
  return (
    IsNotEqual(a.m128_f32[0], b.m128_f32[0]) && IsNotEqual(a.m128_f32[1], b.m128_f32[1])
    && IsNotEqual(a.m128_f32[2], b.m128_f32[2]) && IsNotEqual(a.m128_f32[3], b.m128_f32[3]));
}

inline bool IsValid(const DirectX::XMVECTOR& a)
{
  return (IsValid(a.m128_f32[0]) && IsValid(a.m128_f32[1]) && IsValid(a.m128_f32[2]) && IsValid(a.m128_f32[3]));
}


inline DirectX::XMFLOAT3 HadamardProduct(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
  return DirectX::XMFLOAT3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}

inline DirectX::XMVECTOR HadamardProduct(const DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs)
{
  DirectX::XMFLOAT3 float3(
    lhs.m128_f32[0] * rhs.m128_f32[0],
    lhs.m128_f32[1] * rhs.m128_f32[1],
    lhs.m128_f32[2] * rhs.m128_f32[2]);
  return XMLoadFloat3(&float3);
}

inline DirectX::XMVECTOR HadamardProduct(const DirectX::XMVECTOR& lhs, const DirectX::XMFLOAT3& rhs)
{
  DirectX::XMFLOAT3 float3(lhs.m128_f32[0] * rhs.x, lhs.m128_f32[1] * rhs.y, lhs.m128_f32[2] * rhs.z);
  return XMLoadFloat3(&float3);
}

inline DirectX::XMFLOAT3X3 OuterProduct(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
  return DirectX::XMFLOAT3X3(
    lhs.x * rhs.x,
    lhs.x * rhs.y,
    lhs.x * rhs.z,
    lhs.y * rhs.x,
    lhs.y * rhs.y,
    lhs.y * rhs.z,
    lhs.z * rhs.x,
    lhs.z * rhs.y,
    lhs.z * rhs.z);
}

inline float DotProductVector3(const DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs)
{
  return DirectX::XMVector3Dot(lhs, rhs).m128_f32[0];
}

inline float DotProductVector4(const DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs)
{
  return DirectX::XMVector4Dot(lhs, rhs).m128_f32[0];
}

inline DirectX::XMVECTOR CrossProductTwice(const DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs)
{
  return DirectX::XMVector3Cross(DirectX::XMVector3Cross(lhs, rhs), lhs);
}

inline DirectX::XMVECTOR Rotate(const DirectX::XMVECTOR& orientation, const DirectX::XMVECTOR& vector3)
{
  DirectX::XMVECTOR inverse = DirectX::XMQuaternionInverse(orientation);
  return DirectX::XMVectorSetW(
    DirectX::XMQuaternionMultiply(inverse, DirectX::XMQuaternionMultiply(vector3, orientation)),
    0.0f);
}

inline DirectX::XMVECTOR Rotate(
  const DirectX::XMVECTOR& inverse_orientation,
  const DirectX::XMVECTOR& orientation,
  const DirectX::XMVECTOR& vector3)
{
  return DirectX::XMVectorSetW(
    DirectX::XMQuaternionMultiply(inverse_orientation, DirectX::XMQuaternionMultiply(vector3, orientation)),
    0.0f);
}

inline DirectX::XMVECTOR
  QuaternionFromTo(const DirectX::XMVECTOR& unit_vec3_from, const DirectX::XMVECTOR& unit_vec3_to)
{
  DirectX::XMVECTOR axis = DirectX::XMVector3Cross(unit_vec3_from, unit_vec3_to);
  DirectX::XMVECTOR quaternion = DirectX::XMVectorSet(
    axis.m128_f32[0],
    axis.m128_f32[1],
    axis.m128_f32[2],
    DirectX::XMVector3Dot(unit_vec3_from, unit_vec3_to).m128_f32[0]);
  quaternion = DirectX::XMQuaternionNormalize(quaternion);
  quaternion.m128_f32[3] += 1.0f;
  float from_x = unit_vec3_from.m128_f32[0];
  float from_y = unit_vec3_from.m128_f32[1];
  float from_z = unit_vec3_from.m128_f32[2];
  if (quaternion.m128_f32[3] <= Math::EPSILON)
  {
    if (from_z * from_z > from_x * from_x)
      quaternion = DirectX::XMVectorSet(0.0f, 0.0f, from_z, -from_y);
    else
      quaternion = DirectX::XMVectorSet(0.0f, from_y, -from_x, 0.0f);
  }
  return DirectX::XMQuaternionNormalize(quaternion);
}


} // namespace Math

template<typename... T>
auto sum(T... args)
{
  return (args + ...);
}

template<typename... T>
auto avg(T... args)
{
  return sum(args...) / sizeof...(args);
}

template<typename T>
auto clamp(T x, T min, T max)
{
  T const t = x < min ? min : x;
  return t > max ? max : t;
}

} // namespace Octane
