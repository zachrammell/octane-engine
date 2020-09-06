#pragma once

#include <limits>
#include <algorithm> /* min, max */
#include <array>
#include <utility>   /* forward */

#include <OctaneTest/Helper/TypeWithSize.h>

namespace OctaneTest
{

// std::abs is non constexpr garbage
template<typename T>
constexpr T absolute_value(T const& v)
{
  return v >= T{} ? v : -v;
}

template<typename FloatType>
class IEEE_754
{
public:

  // Constants

  // An unsigned integer of the same size as the floating point number.
  // Used to easily access the raw bit data of the float.
  typedef typename TypeWithSize<sizeof(FloatType)>::UInt BitData;

  // Total number of bits in the float.
  static inline constexpr size_t bitCount = 8 * sizeof(FloatType);

  // Number of bits for the significand in a float.
  static inline constexpr size_t significandBitCount = std::numeric_limits<FloatType>::digits - 1;

  // Number of bits for the exponent in a float. The 1 is for the sign bit.
  static inline constexpr size_t exponentBitCount = bitCount - 1 - significandBitCount;

  // Mask for the sign bit, which is the most significant bit in IEEE-754.
  static inline constexpr BitData signBitMask = BitData(1) << (bitCount - 1);

  // Mask for the significand bits, which are at the least significant end in IEEE-754.
  static inline constexpr BitData significandBitMask = ~BitData(0) >> (exponentBitCount + 1);

  // Mask for the exponent bits, which are between sign and significand bits in IEEE-754.
  static inline constexpr BitData exponentBitMask = ~(signBitMask | significandBitMask);

  // Standard float epsilon, for numbers close to 0.
  static inline constexpr FloatType maxDiff = std::numeric_limits<FloatType>::epsilon();

  // Industry standard(?) number of ULPs to consider close enough
  static inline constexpr size_t maxUlpsDiff = 4;

  // Constructor

  // Construct from a raw floating point value.
  constexpr IEEE_754(FloatType const& f) : data_({ f }) {}

  constexpr IEEE_754(FloatType const&& f) : data_({ f }) {}

  // Construct from another IEEE-754 representation class.
  constexpr IEEE_754(IEEE_754 const&& rhs) : data_({ rhs.float_data() }) {}

  // Conversion operator to a raw floating point value, to enable using test floats in expressions.
  explicit operator FloatType const& () const
  {
    return data_[0].float_;
  }

  // Conversion operator to a raw floating point value, to enable using test floats in expressions.
  explicit operator FloatType& ()
  {
    return data_[0].float_;
  }

  // Non-static methods

  // Returns the exponent bits of this float.
  constexpr BitData exponent_bits() const
  {
    return exponentBitMask & data_[0].bit_;
  }

  // Returns the fraction bits of this float.
  constexpr BitData significand_bits() const
  {
    return significandBitMask & data_[0].bit_;
  }

  // Returns the sign bit of this float.
  constexpr BitData sign_bit() const
  {
    return signBitMask & data_[0].bit_;
  }

  constexpr FloatType float_data() const
  {
    return data_[0].float_;
  }

  constexpr BitData bit_data() const
  {
    return data_[0].bit_;
  }

  // Returns whether a float represents NaN.
  constexpr bool is_nan() const
  {
    // The IEEE-754 standard defines a NaN as a number with all ones in the exponent, and a non-zero significand.
    return (exponent_bits() == exponentBitMask) && (significand_bits() != 0);
  }

  constexpr bool float_close(FloatType const& lhs, FloatType const& rhs) const
  {
    return absolute_value(lhs - rhs) <= maxDiff;
  }

  constexpr bool ulp_close(BitData const& lhs, BitData const& rhs) const
  {
    return lhs - rhs <= maxUlpsDiff;
  }

  // Tests whether two floats are equal or close enough to be considered equal.
  constexpr bool almost_equal(IEEE_754<FloatType> const&& rhs) const
  {
    // NaN are not equal to anything, including other NaN.
    if (is_nan() || rhs.is_nan())
    {
      return false;
    }

    // Check if the numbers are really close -- needed
    // when comparing numbers near zero.
    if (float_close(data_[0].float_, rhs.data_[0].float_))
    {
      return true;
    }

    // Numbers cannot be equal if they do not share the same sign.
    if (sign_bit() != rhs.sign_bit())
    {
      return false;
    }

    // Find the difference in ULPs.
    return ulp_close(data_[0].bit_, rhs.data_[0].bit_);
  }

  // Operator overloads

  constexpr bool operator==(IEEE_754<FloatType> const&& rhs) const
  {
    return almost_equal(std::forward<decltype(rhs)>(rhs));
  }

  constexpr bool operator==(FloatType const&& rhs) const
  {
    return almost_equal(IEEE_754<FloatType>(rhs));
  }

  template<typename FloatType>
  friend constexpr bool operator==(FloatType const&& lhs, IEEE_754<FloatType> const&& rhs);

private:

  union Data
  {
    constexpr Data(FloatType f) : float_(f) {}
    mutable FloatType float_; // mutable so that runtime version can be used
    BitData bit_;
  };

  const std::array<const Data, 1> data_;

};

using t_float = IEEE_754<float>;
using t_double = IEEE_754<double>;

template<typename FloatType>
constexpr bool operator==(FloatType const&& lhs, IEEE_754<FloatType> const&& rhs)
{
  return rhs == lhs;
}

}
