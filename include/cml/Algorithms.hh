#pragma once

#include <set>
#include <type_traits>

#include <boost/multiprecision/cpp_int.hpp>

#include "IsRandomGenerator.hh"
#include "LaunchPolicy.hh"
#include "Mt19937RandomGenerator.hh"

namespace cml {

template <typename T, T _number>
struct IsPowerOf2 {
    static_assert(std::is_arithmetic<T>::value,
                  "Invalid template argument for cml::isPowerOf2: T is not a arithmetic type");

    using type                   = T;
    static constexpr type number = _number;
    static constexpr bool value  = (number != 0) && ((number & (number - 1)) == 0);
};

namespace detail {

template <typename T>
struct ExtendedContainer {
    using Type = std::conditional_t<
        std::is_same<T, std::uint8_t>::value,
        std::uint16_t,
        std::conditional_t<
            std::is_same<T, std::uint16_t>::value,
            std::uint32_t,
            std::conditional_t<
                std::is_same<T, std::uint32_t>::value,
                std::uint64_t,
                std::conditional_t<
                    std::is_same<T, std::uint64_t>::value,
                    boost::multiprecision::uint128_t,
                    std::conditional_t<
                        std::is_same<T, boost::multiprecision::uint128_t>::value,
                        boost::multiprecision::uint256_t,
                        std::conditional_t<
                            std::is_same<T, boost::multiprecision::uint256_t>::value,
                            boost::multiprecision::uint512_t,
                            std::conditional_t<
                                std::is_same<T, boost::multiprecision::uint512_t>::value,
                                boost::multiprecision::uint1024_t,
                                std::conditional_t<
                                    std::is_same<T, std::int8_t>::value,
                                    std::int16_t,
                                    std::conditional_t<
                                        std::is_same<T, std::int16_t>::value,
                                        std::int32_t,
                                        std::conditional_t<
                                            std::is_same<T, std::int32_t>::value,
                                            std::int64_t,
                                            std::conditional_t<
                                                std::is_same<T, std::int64_t>::value,
                                                boost::multiprecision::int128_t,
                                                std::conditional_t<
                                                    std::is_same<T, boost::multiprecision::int128_t>::value,
                                                    boost::multiprecision::int256_t,
                                                    std::conditional_t<
                                                        std::is_same<T, boost::multiprecision::int256_t>::value,
                                                        boost::multiprecision::int512_t,
                                                        std::conditional_t<
                                                            std::is_same<T, boost::multiprecision::int512_t>::value,
                                                            boost::multiprecision::int1024_t,
                                                            boost::multiprecision::cpp_int>>>>>>>>>>>>>>;
};

} // namespace detail

template <typename T>
T modexp(T base, T exp, T modulus)
{
    using namespace boost::multiprecision;

    if (modulus == 1)
        return 0;

    auto newBase = static_cast<typename detail::ExtendedContainer<T>::Type>(base % modulus);
    auto result  = static_cast<typename detail::ExtendedContainer<T>::Type>(1);

    while (exp > 0) {
        if ((exp % 2) == 1)
            result = (result * newBase) % modulus;
        exp     = exp >> 1;
        newBase = (newBase * newBase) % modulus;
    }

    return static_cast<T>(result);
}

template <typename T>
T gcd(T a, T b)
{
    T c = a % b;
    if (c == 0)
        return b;
    return gcd(b, c);
}

template <typename T, class RandomGenerator>
bool millerRabinTest(T number,
                     std::uint32_t k,
                     RandomGenerator &randomGenerator,
                     LaunchPolicy policy = LaunchPolicy::Sync)
{
    static_assert(IsRandomGenerator<RandomGenerator>::value,
                  "Invalid template argument for cml:millerRabinTest(...): RandomGenerator interface is "
                  "not suitable");

    // If n == 2 or n == 3 - number is prime
    if (number == 2 || number == 3)
        return true;

    // If n < 2 or n even
    if (number < 2 || number % 2 == 0)
        return false;

    // Imagine n - 1 as (2^b)*m, where m is odd
    T m             = number - 1;
    std::uint32_t b = 0;

    while (m % 2 == 0) {
        m /= 2;
        ++b;
    }

    for (std::uint32_t i = 0; i < k; i++) {
        // Random integer [2, number - 2]

        T a = static_cast<T>(randomGenerator(2, number - 2));

        // x = a^m mod number
        T x = modexp<T>(a, m, number);

        // If x == 1 or x == n - 1, then go to next iteration
        if (x == 1 || x == number - 1)
            continue;

        for (std::uint32_t r = 1; r < b; r++) {
            // x = x^2 mod number
            x = modexp<T>(x, 2ull, number);

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
template <typename T>
std::set<T> primitiveFactors(T number)
{
    std::set<T> s;
    T n = number - 1;

    if (n % 2 == 0ull) {
        s.insert(2);
        // While i divides n, divide n
        while (n % 2 == 0ull) {
            n = n / 2;
        }
    }

    for (T i = 3; i * i <= n; i = i + 2) {
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
 * \param randomGenerator
 * \param policy Launch policy - need block random generator by mutex or not
 * \param n Modulus
 * \return Primitive root if has, 0 if \a n is primitive or if no roots
 */
template <typename T, class RandomGenerator>
T primitiveRootModulo(T n, RandomGenerator &randomGenerator, LaunchPolicy policy = LaunchPolicy::Sync)
{
    T testable                = n;
    std::uint32_t repeatCount = 0;

    while (testable != 0) {
        testable = testable >> 1;
        ++repeatCount;
    }

    // Check if n is prime or not

    if (!millerRabinTest<T, RandomGenerator>(n, repeatCount, randomGenerator, policy))
        return 0;

    T one = 1;

    if (n == 2)
        return 1;

    // Find value of Euler Totient function of n
    // Since n is a prime number, the value of Euler
    // Totient function is n-1 as there are n-1
    // relatively prime numbers.
    T phi = n - 1;

    // Find prime factors of phi and store in a set
    std::set<T> s = primitiveFactors(n);

    T result = 2;

    for (; result < n; ++result) {
        bool valid = true;
        for (auto it = s.begin(); it != s.end() && valid; ++it) {
            T exp = phi / (*it);
            valid &= modexp(result, exp, n) != one;
        }

        if (valid) {
            return result;
        }
    }

    // If no primitive root found
    return 0;
}

// Euclid extended algorithm
template <typename T>
T gcdex(T a, T b, T &x, T &y)
{
    if (a == 0) {
        x = 0;
        y = 1;
        return b;
    }
    T x1, y1;
    T d = gcdex<T>(b % a, a, x1, y1);
    x   = y1 - (b / a) * x1;
    y   = x1;
    return d;
}

// Inverse modulo
template <typename T>
T invmod(T a, T m)
{
    T x, y;
    gcdex(a, m, x, y);
    x = (x % m + m) % m;
    return x;
}
} // namespace cml