#pragma once

#include <fbi/fbi.hh>
#include <iostream>
#include <set>

#include "Mt19937RandomGenerator.hh"

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

inline std::uint64_t modexp(std::uint64_t base, std::uint64_t exp, std::uint64_t modulus)
{
    if (modulus == 1)
        return 0;

    fbi::BigUnsigned newBase = base % modulus;
    fbi::BigUnsigned result  = 1;

    while (exp > 0) {
        if ((exp & 1) == 1)
            result = (result * newBase) % modulus;
        exp     = exp >> 1;
        newBase = (newBase * newBase) % modulus;
    }

    return result.toUnsignedLongLong();
}

inline std::uint64_t gcd(std::uint64_t a, std::uint64_t b)
{
    std::uint64_t c = a % b;
    if (c == 0)
        return b;
    return gcd(b, c);
}

template <class RandomGenerator = Mt19937RandomGenerator>
bool millerRabinTest(std::uint64_t number, std::uint32_t k, RandomGenerator randomGenerator = RandomGenerator{})
{
    // If n == 2 or n == 3 - number is prime
    if (number == 2 || number == 3)
        return true;

    // If n < 2 or n even
    if (number < 2 || number % 2 == 0)
        return false;

    // Imagine n - 1 as (2^b)*m, where m is odd
    std::uint64_t m = number - 1;
    std::uint32_t b = 0;

    while (m % 2 == 0) {
        m /= 2;
        ++b;
    }

    for (std::uint32_t i = 0; i < k; i++) {
        // Random integer [2, number - 2]

        std::uint64_t a = randomGenerator(2, number - 2);

        // x = a^m mod number
        std::uint64_t x = modexp(a, m, number);

        // If x == 1 or x == n - 1, then go to next iteration
        if (x == 1 || x == number - 1)
            continue;

        for (std::uint32_t r = 1; r < b; r++) {
            // x = x^2 mod number
            x = modexp(x, 2, number);

            // If x == 1, then return "complex number"
            if (x == 1)
                return false;

            // If x == n - 1, then go next iteration outside loop
            if (x == number - 1)
                break;
        }

        if (x != number - 1)
            return false;
    }

    // Return "probably prime"
    return true;
}

// Utility function to store prime factors of a number
inline std::set<std::uint64_t> primitiveFactors(std::uint64_t number)
{
    std::set<std::uint64_t> s;
    std::uint64_t n = number - 1;

    if (n % 2 == 0ull) {
        s.insert(2);
        // While i divides n, divide n
        while (n % 2 == 0ull) {
            n = n / 2;
        }
    }

    //// Sqrt of UINT_MAX
    // if (n > 4294967296)
    //    throw std::overflow_error{ "mech::crypt::primitiveFactors(...): argument is too big" };

    for (std::uint64_t i = 3; i * i <= n; i = i + 2) {
        if (n % i == 0ull) {
            s.insert(i);
            // While i divides n, divide n
            while (n % i == 0ull) {
                n = n / i;
            }
        }
    }

    // This condition is to handle the case when
    // n is a prime number greater than 2
    if (n > 1)
        s.insert(n);

    return s;
}

/**
 * \brief Function to find smallest primitive root of n
 * \tparam T Return type
 * \param p Modulus
 * \return Primitive root if has, 0 if \a p is primitive or if no roots
 */
inline std::uint64_t primitiveRootModulo(std::uint64_t n)
{
    std::uint64_t testable    = n;
    std::uint32_t repeatCount = 0;

    while (testable != 0) {
        testable = testable >> 1;
        ++repeatCount;
    }

    // Check if n is prime or not
    if (!millerRabinTest(n, repeatCount))
        return 0;

    std::uint64_t one = 1;

    if (n == 2)
        return 1;

    // Find value of Euler Totient function of n
    // Since n is a prime number, the value of Euler
    // Totient function is n-1 as there are n-1
    // relatively prime numbers.
    std::uint64_t phi = n - 1;

    // Find prime factors of phi and store in a set
    std::set<std::uint64_t> s = primitiveFactors(n);

    std::uint64_t result = 2;

    for (; result < n; ++result) {
        bool valid = true;
        for (auto it = s.begin(); it != s.end() && valid; ++it) {
            std::uint64_t exp = phi / (*it);
            valid &= modexp(result, exp, n) != one;
        }

        if (valid) {
            return result;
        }
    }

    // If no primitive root found
    return 0;
}
} // namespace crypt
} // namespace mech