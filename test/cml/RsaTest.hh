#pragma once

#include <sstream>

#include <cml/cml.hh>
#include <gtest/gtest.h>

using namespace mech::crypt;

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

void testRsa(bool print = false)
{
    using RandomGenerator = Mt19937RandomGenerator;
    using PrimeGenerator  = MilRabPrimeGenerator<50>;

    RsaProtocol aliceRsaGenerator{};
    RsaProtocol bobRsaGenerator{};

    aliceRsaGenerator.generate();
    bobRsaGenerator.generate();

    std::vector<std::uint64_t> aliceMessage{ 'A', 'l', 'i', 'c', 'e', '\0' };
    std::vector<std::uint64_t> bobMessage{ 'B', 'o', 'b', '\0' };

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
        std::cout << "======================================================================" << std::endl;
        std::cout << std::endl << std::endl << std::endl;
    }

    EXPECT_EQ(aliceMessage, bobReceivedMessage);
    EXPECT_EQ(bobMessage, aliceReceivedMessage);
}

TEST(RsaProtocol, InWork)
{
    for (std::size_t i = 0; i < 100; ++i) {
        testRsa(false);
    }
}