#pragma once

#include "Algorithms.hh"
#include "IsPrimeGenerator.hh"
#include "IsRandomGenerator.hh"

namespace mech {
namespace crypt {

/* ================================================================================= */
/* ============================ DiffieHellmanPublicKey ============================= */
/* ================================================================================= */

template <class PrimeGeneratorType>
struct DiffieHellmanSecurityBase {
    static_assert(IsPrimeGenerator<PrimeGeneratorType>::value,
                  "Invalid template argument for mech::crypt::DiffieHellmanSecurityBase: PrimeGeneratorType "
                  "interface is not suitable");

    using PrimeGenerator = PrimeGeneratorType;

    DiffieHellmanSecurityBase();
    explicit DiffieHellmanSecurityBase(const PrimeGenerator& primeGenerator);

    DiffieHellmanSecurityBase& generate();

    std::uint64_t g{ 0 };
    std::uint64_t p{ 0 };
    PrimeGenerator primeGenerator{};
};

template <class PrimeGeneratorType>
DiffieHellmanSecurityBase<PrimeGeneratorType>::DiffieHellmanSecurityBase() = default;

template <class PrimeGeneratorType>
DiffieHellmanSecurityBase<PrimeGeneratorType>::DiffieHellmanSecurityBase(const PrimeGenerator& primeGenerator) :
    primeGenerator(primeGenerator)
{}

template <class PrimeGeneratorType>
DiffieHellmanSecurityBase<PrimeGeneratorType>& DiffieHellmanSecurityBase<PrimeGeneratorType>::generate()
{
    this->p = primeGenerator();
    this->g = primitiveRootModulo(this->p);
    return *this;
}

struct DiffieHellmanPublicKey {
    std::uint64_t v{ 0 };
};

struct DiffieHellmanPrivateKey {
    std::uint64_t a{ 0 };
};

/* ================================================================================= */
/* ============================= DiffieHellmanProtocol ============================= */
/* ================================================================================= */

template <class SecurityBaseType, class RandomGeneratorType = Mt19937RandomGenerator>
struct DiffieHellmanProtocol {
    static_assert(IsRandomGenerator<RandomGeneratorType>::value,
                  "Invalid template argument for mech::crypt::DiffieHellmanProtocol: "
                  "RandomGeneratorType interface is not suitable");

    using PublicKey       = DiffieHellmanPublicKey;
    using PrivateKey      = DiffieHellmanPrivateKey;
    using SecurityBase    = SecurityBaseType;
    using RandomGenerator = RandomGeneratorType;

    explicit DiffieHellmanProtocol(SecurityBase base);
    DiffieHellmanProtocol(SecurityBase base, const RandomGenerator& randomGenerator);

    DiffieHellmanProtocol& generate();

    PublicKey publicKey{};
    PrivateKey privateKey{};
    SecurityBase securityBase{};
    RandomGenerator randomGenerator{};
};

template <class SecurityBaseType, class RandomGeneratorType>
DiffieHellmanProtocol<SecurityBaseType, RandomGeneratorType>::DiffieHellmanProtocol(SecurityBase base) :
    securityBase(base)
{}

template <class SecurityBaseType, class RandomGeneratorType>
DiffieHellmanProtocol<SecurityBaseType, RandomGeneratorType>::DiffieHellmanProtocol(
    SecurityBase base,
    const RandomGenerator& randomGenerator) :
    securityBase(base),
    randomGenerator(randomGenerator)
{}

template <class SecurityBaseType, class RandomGeneratorType>
DiffieHellmanProtocol<SecurityBaseType, RandomGeneratorType>& DiffieHellmanProtocol<SecurityBaseType,
    RandomGeneratorType>::generate()
{
    if (securityBase.g == 0 || securityBase.p == 0)
        throw std::logic_error{ "Security base is not generated" };

    privateKey.a = randomGenerator();
    publicKey.v  = modexp(securityBase.g, privateKey.a, securityBase.p).toUnsignedLongLong();
    return *this;
}
} // namespace crypt
} // namespace mech