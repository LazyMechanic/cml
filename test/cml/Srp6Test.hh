#pragma once

#include <cml/cml.hh>
#include <gtest/gtest.h>

using namespace cml;

template <Uint32 bitness, typename Value, class SecurityBase, class RandomGenerator, class SafePrimeGenerator>
void testSrp6(const SecurityBase& securityBase,
              RandomGenerator& randomGenerator,
              SafePrimeGenerator& safePrimeGenerator,
              bool print = false)
{
    using Client = Srp6Client<Value, RandomGenerator>;
    using Server = Srp6Server<Value, RandomGenerator>;

    using ServerData       = Srp6ServerData<Value>;
    using ServerPublicKey  = Srp6ServerPublicKey<Value>;
    using ServerPrivateKey = Srp6ServerPrivateKey;
    using ClientPublicKey  = Srp6ClientPublicKey<Value>;
    using ClientPrivateKey = Srp6ClientPrivateKey;

    std::string clientIdentifier = "Login";
    std::string clientPassword   = "Password";

    ServerData serverData = ServerData::create(randomGenerator, securityBase, clientPassword);

    Client client{};
    Server server{};

    Value a = randomGenerator();
    Value b = randomGenerator();

    auto clientPublicKey = client.generatePublicKey(securityBase, a, clientIdentifier);
    auto serverPublicKey = server.generatePublicKey(securityBase, b, serverData);

    auto clientPrivateKey =
        client.generatePrivateKey(securityBase, clientPassword, a, clientPublicKey, serverPublicKey);
    auto serverPrivateKey = server.generatePrivateKey(securityBase, serverData, b, serverPublicKey, clientPublicKey);

    if (print) {
        std::cout << "Client public key [A]:    " << clientPublicKey.A << std::endl;
        std::cout << "Client public key [id]:   " << clientPublicKey.identifier << std::endl;
        std::cout << "Client random number [a]: " << a << std::endl;

        std::cout << "Server public key [B]:    " << serverPublicKey.B << std::endl;
        std::cout << "Server public key [salt]: " << serverPublicKey.salt << std::endl;
        std::cout << "Server random number [b]: " << b << std::endl;

        std::cout << "Client private key [K]:   " << clientPrivateKey.K << std::endl;
        std::cout << "Server private key [K]:   " << serverPrivateKey.K << std::endl;

        std::cout
            << "======================================================================================================="
            << std::endl;
    }

    EXPECT_EQ(clientPrivateKey.K, serverPrivateKey.K);
}

template <Uint32 bitness>
void multipleSrp6Tests(std::size_t testsAmount, bool print = false)
{
    using Value              = typename ContainerByBitness<bitness>::Type;
    using RandomGenerator    = Mt19937RandomGenerator<bitness, Value>;
    using PrimeGenerator     = MilRabPrimeGenerator<bitness, RandomGenerator, Value>;
    using SafePrimeGenerator = MilRabSafePrimeGenerator<PrimeGenerator, RandomGenerator, Value>;
    using SecurityBase       = Srp6SecurityBase<Value>;

    SafePrimeGenerator safePrimeGenerator{};
    RandomGenerator randomGenerator{};

    SecurityBase securityBase = SecurityBase::create(safePrimeGenerator, randomGenerator);

    if (print) {
        std::cout << "Security base [N]: " << securityBase.N << std::endl;
        std::cout << "Security base [g]: " << securityBase.g << std::endl;
        std::cout << "Security base [k]: " << securityBase.k << std::endl;
        std::cout
            << "======================================================================================================="
            << std::endl;
    }

    for (std::size_t i = 0; i < testsAmount; ++i) {
        testSrp6<bitness, Value>(securityBase, randomGenerator, safePrimeGenerator, print);
    }
}

TEST(Srp6Protocol, InWork_50_x10_print)
{
    constexpr Uint32 bitness = 50;
    std::size_t testsAmount  = 10;
    bool print               = true;

    multipleSrp6Tests<bitness>(testsAmount, print);
}

TEST(Srp6Protocol, InWork_30_x100)
{
    constexpr Uint32 bitness = 30;
    std::size_t testsAmount  = 100;
    bool print               = false;

    multipleSrp6Tests<bitness>(testsAmount, print);
}

TEST(Srp6Protocol, InWork_50_x100)
{
    constexpr Uint32 bitness = 50;
    std::size_t testsAmount  = 100;
    bool print               = false;

    multipleSrp6Tests<bitness>(testsAmount, print);
}