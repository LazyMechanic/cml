#pragma once

#include <type_traits>

#include "IsPowerOf2.hh"

namespace mech {
namespace crypt {
template <typename T>
T modpow(T base, T exp, T modulus)
{
    base %= modulus;
    T result = 1;
    while (exp > 0) {
        if ((exp & 1) == 1)
            result = (result * base) % modulus;
        base = (base * base) % modulus;
        exp >>= 1;
    }

    return result;
}
} // namespace crypt
} // namespace mech