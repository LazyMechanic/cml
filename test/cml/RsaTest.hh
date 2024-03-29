#pragma once

#include <sstream>

#include <cml/cml.hh>
#include <gtest/gtest.h>

using namespace cml;

template <typename T>
std::string vectorToString(const std::vector<T>& message)
{
    std::stringstream ss;
    ss << "[ ";
    for (auto it = message.begin(), preend = std::prev(message.end()); it != message.end(); ++it) {
        ss << *it;
        if (it != preend) {
            ss << ", ";
        }
    }
    ss << " ]";
    return ss.str();
}

template <uint32_t bitness>
void testRsa(bool print = false)
{
    using Value           = typename ContainerByBitness<bitness>::Type;
    using RandomGenerator = Mt19937RandomGenerator<bitness, Value>;
    using PrimeGenerator  = MilRabPrimeGenerator<bitness, RandomGenerator, Value>;
    using Protocol        = RsaProtocol<PrimeGenerator>;

    Protocol aliceRsaGenerator{};
    Protocol bobRsaGenerator{};

    aliceRsaGenerator.generate();
    bobRsaGenerator.generate();

    std::vector<uint64_t> aliceMessage{ 'A', 'l', 'i', 'c', 'e', '\0' };
    std::vector<uint64_t> bobMessage{ 'B', 'o', 'b', '\0' };

    auto aliceEncryptedMessage = aliceRsaGenerator.encrypt(aliceMessage, bobRsaGenerator.publicKey);
    auto bobEncryptedMessage   = bobRsaGenerator.encrypt(bobMessage, aliceRsaGenerator.publicKey);

    auto aliceReceivedMessage = aliceRsaGenerator.decrypt(bobEncryptedMessage);
    auto bobReceivedMessage   = bobRsaGenerator.decrypt(aliceEncryptedMessage);

    if (print) {
        std::cout << "Alice original message: " << vectorToString(aliceMessage) << std::endl;
        std::cout << "Bob original message:   " << vectorToString(bobMessage) << std::endl;
        std::cout << "----------------------------------------------------------------------" << std::endl;
        std::cout << "Alice encrypted message: " << vectorToString(aliceEncryptedMessage) << std::endl;
        std::cout << "Bob encrypted message:   " << vectorToString(bobEncryptedMessage) << std::endl;
        std::cout << "----------------------------------------------------------------------" << std::endl;
        std::cout << "Alice received and decrypt message: " << vectorToString(aliceReceivedMessage) << std::endl;
        std::cout << "Bob received and decrypt message:   " << vectorToString(bobReceivedMessage) << std::endl;
        std::cout
            << "======================================================================================================="
            << std::endl;
    }

    EXPECT_EQ(aliceMessage, bobReceivedMessage);
    EXPECT_EQ(bobMessage, aliceReceivedMessage);
}

TEST(RsaProtocol, InWork_64_x10_print)
{
    constexpr uint32_t bitness = 64;
    bool print                 = true;

    for (std::size_t i = 0; i < 10; ++i) {
        testRsa<bitness>(print);
    }
}

TEST(RsaProtocol, InWork_16_x100)
{
    constexpr uint32_t bitness = 16;
    bool print                 = false;

    for (std::size_t i = 0; i < 100; ++i) {
        testRsa<bitness>(print);
    }
}

TEST(RsaProtocol, InWork_32_x100)
{
    constexpr uint32_t bitness = 32;
    bool print                 = false;

    for (std::size_t i = 0; i < 100; ++i) {
        testRsa<bitness>(print);
    }
}

TEST(RsaProtocol, InWork_64_x100)
{
    constexpr uint32_t bitness = 64;
    bool print                 = false;

    for (std::size_t i = 0; i < 100; ++i) {
        testRsa<bitness>(print);
    }
}

TEST(RsaProtocol, InWork_128_x10)
{
    constexpr uint32_t bitness = 128;
    bool print                 = false;

    for (std::size_t i = 0; i < 10; ++i) {
        testRsa<bitness>(print);
    }
}