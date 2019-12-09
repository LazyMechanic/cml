#pragma once

#include <cml/cml.hh>
#include <gtest/gtest.h>

using namespace cml;

template <uint32_t bitness>
void testDiffieHellman(bool print = false)
{
    using Value           = typename ContainerByBitness<bitness>::Type;
    using RandomGenerator = Mt19937RandomGenerator<bitness, Value>;
    using PrimeGenerator  = MilRabPrimeGenerator<bitness, RandomGenerator, Value>;
    using SecurityBase    = DiffieHellmanSecurityBase<Value>;
    using Protocol        = DiffieHellmanProtocol<Value, RandomGenerator>;

    PrimeGenerator primeGenerator{};
    RandomGenerator randomGenerator{};
    SecurityBase base = SecurityBase::create(primeGenerator, randomGenerator);

    Protocol aliceKeyGenerator{};
    Protocol bobKeyGenerator{};

    aliceKeyGenerator.generate(base);
    bobKeyGenerator.generate(base);

    auto aliceKey = aliceKeyGenerator.publicKey;
    auto bobKey   = bobKeyGenerator.publicKey;

    auto aliceReceived = modexp(bobKey.v, aliceKeyGenerator.privateKey.a, base.p);
    auto bobReceived   = modexp(aliceKey.v, bobKeyGenerator.privateKey.a, base.p);

    if (print) {
        std::cout << "SecurityBase.g: " << base.g << std::endl;
        std::cout << "SecurityBase.p: " << base.p << std::endl;

        std::cout << "Alice key: " << aliceKey.v << std::endl;
        std::cout << "Bob key:   " << bobKey.v << std::endl;

        std::cout << "Alice received: " << aliceReceived << std::endl;
        std::cout << "Bob received:   " << bobReceived << std::endl;

        std::cout
            << "======================================================================================================="
            << std::endl;
    }

    EXPECT_EQ(aliceReceived, bobReceived);
}

TEST(DiffieHellmanProtocol, InWork_50_x10_print)
{
    constexpr uint32_t bitness = 50;

    for (std::size_t i = 0; i < 10; ++i) {
        testDiffieHellman<bitness>(true);
    }
}

TEST(DiffieHellmanProtocol, InWork_16_x100)
{
    constexpr uint32_t bitness = 16;

    for (std::size_t i = 0; i < 100; ++i) {
        testDiffieHellman<bitness>(false);
    }
}

TEST(DiffieHellmanProtocol, InWork_32_x100)
{
    constexpr uint32_t bitness = 32;

    for (std::size_t i = 0; i < 100; ++i) {
        testDiffieHellman<bitness>(false);
    }
}

TEST(DiffieHellmanProtocol, InWork_40_x100)
{
    constexpr uint32_t bitness = 40;

    for (std::size_t i = 0; i < 100; ++i) {
        testDiffieHellman<bitness>(false);
    }
}

TEST(DiffieHellmanProtocol, InWork_50_x100)
{
    constexpr uint32_t bitness = 50;

    for (std::size_t i = 0; i < 100; ++i) {
        testDiffieHellman<bitness>(false);
    }
}

TEST(DiffieHellmanProtocol, InWork_60_x50)
{
    constexpr uint32_t bitness = 60;

    for (std::size_t i = 0; i < 50; ++i) {
        testDiffieHellman<bitness>(false);
    }
}