#pragma once

#include <type_traits>

namespace Octane
{

template<typename E>
constexpr auto to_integral(E e)
{
  return static_cast<std::underlying_type_t<E>>(e);
}

} // namespace Octane
