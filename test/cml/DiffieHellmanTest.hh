#pragma once

#include <cml/cml.hh>
#include <gtest/gtest.h>

using namespace mech::crypt;

void testDiffieHellman(bool print = false)
{
    using PrimeGenerator = MilRabPrimeGenerator<50>;
    using RandomGenerator = Mt19937RandomGenerator;
    using SecurityBase = DiffieHellmanSecurityBase<PrimeGenerator>;

    SecurityBase base{};

    base.generate();

    DiffieHellmanProtocol aliceKeyGenerator{ base };
    DiffieHellmanProtocol bobKeyGenerator{ base };

    auto aliceKey = aliceKeyGenerator.generate().publicKey;
    auto bobKey   = bobKeyGenerator.generate().publicKey;

    auto aliceReceived = modexp(bobKey.v, aliceKeyGenerator.privateKey.a, base.p);
    auto bobReceived   = modexp(aliceKey.v, bobKeyGenerator.privateKey.a, base.p);

    if (print) {
        std::cout << "SecurityBase.g: " << base.g << std::endl;
        std::cout << "SecurityBase.p: " << base.p << std::endl;

        std::cout << "Alice key: " << aliceKey.v << std::endl;
        std::cout << "Bob key:   " << bobKey.v << std::endl;

        std::cout << "Alice received: " << aliceReceived << std::endl;
        std::cout << "Bob received:   " << bobReceived << std::endl;
    }

    EXPECT_EQ(aliceReceived, bobReceived);
}

TEST(DiffieHellmanProtocol, InWork)
{
    for (std::size_t i = 0; i < 100; ++i) {
        testDiffieHellman(false);
    }
}