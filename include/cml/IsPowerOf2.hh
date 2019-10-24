#pragma once
#include <type_traits>

namespace mech {
namespace crypt {
template <typename T, T _number>
struct IsPowerOf2 {
    static_assert(std::is_arithmetic<T>::value,
                  "Invalid template argument for mech::crypt::isPowerOf2: T is not a arithmetic type");

    using type                   = T;
    static constexpr type number = _number;
    static constexpr bool value  = (number != 0) && ((number & (number - 1)) == 0);
};
} // namespace crypt
} // namespace mech