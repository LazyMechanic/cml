#pragma once

#include <cml/cml.hh>
#include <gtest/gtest.h>

using namespace mech::crypt;

void testDiffieHellman(bool print = false)
{
    using ResultType    = std::uint64_t;
    using PublicKeyType = DiffieHellmanPublicKey;

    PublicKeyType publicKey = PublicKeyType{ MilRabPrimeGenerator<50>{} };

    DiffieHellmanProtocol aliceKeyGenerator{};
    DiffieHellmanProtocol bobKeyGenerator{};

    auto aliceKey = aliceKeyGenerator.generate(publicKey);
    auto bobKey   = bobKeyGenerator.generate(publicKey);

    auto aliceReceived = modexp(bobKey, aliceKeyGenerator.getA(), publicKey.p);
    auto bobReceived   = modexp(aliceKey, bobKeyGenerator.getA(), publicKey.p);

    if (print) {
        std::cout << "PublicKey.g: " << publicKey.g << std::endl;
        std::cout << "PublicKey.p: " << publicKey.p << std::endl;

        std::cout << "Alice key: " << aliceKey << std::endl;
        std::cout << "Bob key:   " << bobKey << std::endl;

        std::cout << "Alice received: " << aliceReceived << std::endl;
        std::cout << "Bob received:   " << bobReceived << std::endl;
    }

    EXPECT_EQ(aliceReceived, bobReceived);
}

TEST(DiffieHellmanProtocol, Received)
{
    for (std::size_t i = 0; i < 100; ++i) {
        testDiffieHellman(true);
    }
}