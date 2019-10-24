#pragma once

#include <array>
#include <bitset>
#include <cstdint>
#include <iostream>

#include "DefaultRandomGenerator.hh"
#include "IsRandomGenerator.hh"
#include "Math.hh"
#include "PrimeGenerator.hh"

namespace mech {
namespace crypt {
template <std::uint32_t primeDimension,
          typename BigUint,
          class RandomGeneratorType = DefaultRandomGenerator<primeDimension, BigUint>>
class MilRabPrimeGenerator : public PrimeGenerator<primeDimension, BigUint> {
public:
    static_assert(IsRandomGenerator<RandomGeneratorType>::value,
                  "Invalid template argument for mech::crypt::MilRabPrimeGenerator: RandomGeneratorType interface is "
                  "not suitable");

    using RandomGenerator                          = RandomGeneratorType;
    using Base                                     = PrimeGenerator<primeDimension, BigUint>;
    using Result                                   = typename Base::Result;
    static constexpr std::uint32_t resultDimension = Base::resultDimension;

    MilRabPrimeGenerator();

    explicit MilRabPrimeGenerator(const RandomGenerator& randomGenerator);

    Result generate() override;

private:
    static constexpr std::array<std::uint32_t, 303> smallPrimes{
        2,    3,    5,    7,    11,   13,   17,   19,   23,   29,   31,   37,   41,   43,   47,   53,   59,   61,
        67,   71,   73,   79,   83,   89,   97,   101,  103,  107,  109,  113,  127,  131,  137,  139,  149,  151,
        157,  163,  167,  173,  179,  181,  191,  193,  197,  199,  211,  223,  227,  229,  233,  239,  241,  251,
        257,  263,  269,  271,  277,  281,  283,  293,  307,  311,  313,  317,  331,  337,  347,  349,  353,  359,
        367,  373,  379,  383,  389,  397,  401,  409,  419,  421,  431,  433,  439,  443,  449,  457,  461,  463,
        467,  479,  487,  491,  499,  503,  509,  521,  523,  541,  547,  557,  563,  569,  571,  577,  587,  593,
        599,  601,  607,  613,  617,  619,  631,  641,  643,  647,  653,  659,  661,  673,  677,  683,  691,  701,
        709,  719,  727,  733,  739,  743,  751,  757,  761,  769,  773,  787,  797,  809,  811,  821,  823,  827,
        829,  839,  853,  857,  859,  863,  877,  881,  883,  887,  907,  911,  919,  929,  937,  941,  947,  953,
        967,  971,  977,  983,  991,  997,  1009, 1013, 1019, 1021, 1031, 1033, 1039, 1049, 1051, 1061, 1063, 1069,
        1087, 1091, 1093, 1097, 1103, 1109, 1117, 1123, 1129, 1151, 1153, 1163, 1171, 1181, 1187, 1193, 1201, 1213,
        1217, 1223, 1229, 1231, 1237, 1249, 1259, 1277, 1279, 1283, 1289, 1291, 1297, 1301, 1303, 1307, 1319, 1321,
        1327, 1361, 1367, 1373, 1381, 1399, 1409, 1423, 1427, 1429, 1433, 1439, 1447, 1451, 1453, 1459, 1471, 1481,
        1483, 1487, 1489, 1493, 1499, 1511, 1523, 1531, 1543, 1549, 1553, 1559, 1567, 1571, 1579, 1583, 1597, 1601,
        1607, 1609, 1613, 1619, 1621, 1627, 1637, 1657, 1663, 1667, 1669, 1693, 1697, 1699, 1709, 1721, 1723, 1733,
        1741, 1747, 1753, 1759, 1777, 1783, 1787, 1789, 1801, 1811, 1823, 1831, 1847, 1861, 1867, 1871, 1873, 1877,
        1879, 1889, 1901, 1907, 1913, 1931, 1933, 1949, 1951, 1973, 1979, 1987, 1993, 1997, 1999
    };

    RandomGenerator m_randomGenerator{};

    static bool isDividedBySmallPrimes(Result number);

    bool millerRabinTest(Result number, std::uint32_t k);
};

template <std::uint32_t primeDimension, typename BigUint, class RandomGeneratorType>
MilRabPrimeGenerator<primeDimension, BigUint, RandomGeneratorType>::MilRabPrimeGenerator() = default;

template <std::uint32_t primeDimension, typename BigUint, class RandomGeneratorType>
MilRabPrimeGenerator<primeDimension, BigUint, RandomGeneratorType>::MilRabPrimeGenerator(
    const RandomGenerator& randomGenerator) :
    m_randomGenerator(randomGenerator)
{}

template <std::uint32_t primeDimension, typename BigUint, class RandomGeneratorType>
typename MilRabPrimeGenerator<primeDimension, BigUint, RandomGeneratorType>::Result
    MilRabPrimeGenerator<primeDimension, BigUint, RandomGeneratorType>::generate()
{
    Result prime = 0;

    constexpr std::uint32_t testRepeatCount = resultDimension;
    
    while (true) {
        BigUint maxLimit{1};
        maxLimit = maxLimit << (resultDimension + 1) - 1; 

        // clang-format off
        prime = m_randomGenerator(
            2,
            maxLimit);
        // clang-format on

        prime |= 0x1;
        prime |= BigUint{ 0x1 } << (resultDimension - 1);

        // If number is not prime then generate next number
        if (isDividedBySmallPrimes(prime))
            continue;

        if (millerRabinTest(prime, testRepeatCount))
            break;
    }
    return prime;
}

template <std::uint32_t primeDimension, typename BigUint, class RandomGeneratorType>
bool MilRabPrimeGenerator<primeDimension, BigUint, RandomGeneratorType>::isDividedBySmallPrimes(Result number)
{
    for (auto&& prime : smallPrimes) {
        if (number % prime == 0)
            return true;
    }

    return false;
}

template <std::uint32_t primeDimension, typename BigUint, class RandomGeneratorType>
bool MilRabPrimeGenerator<primeDimension, BigUint, RandomGeneratorType>::millerRabinTest(Result number, std::uint32_t k)
{
    // If n == 2 or n == 3 - number is prime
    if (number == 2 || number == 3)
        return true;

    // If n < 2 or n even
    if (number < 2 || number % 2 == 0)
        return false;

    // Imagine n − 1 as (2^b)·m, where m is odd
    Result m        = number - 1;
    std::uint32_t b = 0;

    while (m % 2 == 0) {
        m /= 2;
        ++b;
    }

    for (std::uint32_t i = 0; i < k; i++) {
        // Random integer [2, number − 2]

        Result a = m_randomGenerator(2, number - 2);

        // x = a^m mod number
        Result x = modpow<Result>(a, m, number);

        // If x == 1 or x == n − 1, then go to next iteration
        if (x == 1 || x == number - 1)
            continue;

        for (std::uint32_t r = 1; r < b; r++) {
            // x = x^2 mod number
            x = modpow<Result>(x, 2, number);

            // If x == 1, then return "complex number"
            if (x == 1)
                return false;

            // If x == n − 1, then go next iteration outside loop
            if (x == number - 1)
                break;
        }

        if (x != number - 1)
            return false;
    }

    // Return "probably prime"
    return true;
}

// template <typename limitInteger>
// constexpr std::bitset<digits> MilRabPrimeGenerator<BigUint, digits, ResultType, RandomGenerator>::fullSetBits()
//{
//    return std::bitset<digits>{ std::numeric_limits<limitInteger>::max() };
//}
//
// template <typename BigUint, std::int32_t digits, typename ResultType, class RandomGenerator>
// MilRabPrimeGenerator<BigUint, digits, ResultType, RandomGenerator>::MilRabPrimeGenerator() :
// MilRabPrimeGenerator(std::random_device{}())
//{}
//
// template <typename BigUint, std::int32_t digits, typename ResultType, class RandomGenerator>
// MilRabPrimeGenerator<BigUint, digits, ResultType, RandomGenerator>::MilRabPrimeGenerator(ResultType seed) :
// m_randomGenerator(seed)
//{}
//
// template <typename BigUint, std::int32_t digits, typename ResultType, class RandomGenerator>
// ResultType MilRabPrimeGenerator<BigUint, digits, ResultType, RandomGenerator>::operator()()
//{
//    return generatePrime();
//}
//
// template <typename BigUint, std::int32_t digits, typename ResultType, class RandomGenerator>
// ResultType MilRabPrimeGenerator<BigUint, digits, ResultType, RandomGenerator>::randomInteger(ResultType min,
// ResultType max)
//{
//    return std::uniform_int_distribution<ResultType>{ min, max }(m_randomGenerator);
//}
//
// template <typename BigUint, std::int32_t digits, typename ResultType, class RandomGenerator>
// bool MilRabPrimeGenerator<BigUint, digits, ResultType, RandomGenerator>::isDividedBySmallPrimes(ResultType number)
//{
//    for (auto&& prime : smallPrimes) {
//        if (number % prime == 0)
//            return true;
//    }
//
//    return false;
//}
//
// template <typename BigUint, std::int32_t digits, typename ResultType, class RandomGenerator>
// BigUint MilRabPrimeGenerator<BigUint, digits, ResultType, RandomGenerator>::modpow(BigUint base, BigUint exp, BigUint
// modulus)
//{
//    base %= modulus;
//    BigUint result = 1;
//    while (exp > 0) {
//        if ((exp & 1) == 1)
//            result = (result * base) % modulus;
//        base = (base * base) % modulus;
//        exp >>= 1;
//    }
//
//    return result;
//}
//
// template <typename BigUint, std::int32_t digits, typename ResultType, class RandomGenerator>
// bool MilRabPrimeGenerator<BigUint, digits, ResultType, RandomGenerator>::millerRabinTest(ResultType number,
// std::int32_t k)
//{
//    // If n == 2 or n == 3 - number is prime
//    if (number == 2 || number == 3)
//        return true;
//
//    // If n < 2 or n even
//    if (number < 2 || number % 2 == 0)
//        return false;
//
//    // Imagine n − 1 as (2^b)·m, where m is odd
//    ResultType m = number - 1;
//    ResultType b = 0;
//
//    while (m % 2 == 0) {
//        m /= 2;
//        ++b;
//    }
//
//    for (std::int32_t i = 0; i < k; i++) {
//        // Random integer [2, number − 2]
//        ResultType a = randomInteger(2, number - 2);
//
//        // x = a^m mod number
//        BigUint x = modpow(a, m, number);
//
//        // If x == 1 or x == n − 1, then go to next iteration
//        if (x == 1 || x == number - 1)
//            continue;
//
//        for (std::int32_t r = 1; r < b; r++) {
//            // x = x^2 mod number
//            x = modpow(x, 2, number);
//
//            // If x == 1, then return "complex number"
//            if (x == 1)
//                return false;
//
//            // If x == n − 1, then go next iteration outside loop
//            if (x == number - 1)
//                break;
//        }
//
//        if (x != number - 1)
//            return false;
//    }
//
//    // Return "probably prime"
//    return true;
//}
//
// template <typename BigUint, std::int32_t digits, typename ResultType, class RandomGenerator>
// std::uint64_t MilRabPrimeGenerator<BigUint, digits, ResultType, RandomGenerator>::generatePrime()
//{
//    ResultType prime = 0;
//
//    const std::int32_t testRepeatCount = digits;
//
//    while (true) {
//        // clang-format off
//        prime = randomInteger(
//            2ull,
//            fullSetBits<ResultType>().to_ullong());
//        // clang-format on
//
//        prime |= 0x1;
//        prime |= 0x1ull << (digits - 1);
//
//        // If number is not prime then generate next number
//        if (isDividedBySmallPrimes(prime))
//            continue;
//
//        if (millerRabinTest(prime, testRepeatCount))
//            break;
//    }
//
//    return prime;
//}
} // namespace crypt
} // namespace mech