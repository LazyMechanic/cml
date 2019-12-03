#pragma once

#include "IsPrimeGenerator.hh"
#include "IsRandomGenerator.hh"

namespace cml {

/* ================================================================================= */
/* ============================ DiffieHellmanPublicKey ============================= */
/* ================================================================================= */

template <typename ValueType>
struct DiffieHellmanSecurityBase {
    using Value = ValueType;

    template <class PrimeGeneratorType, class RandomGeneratorType>
    static DiffieHellmanSecurityBase create(PrimeGeneratorType& primeGenerator, RandomGeneratorType& randomGenerator);

    Value g{ 0 };
    Value p{ 0 };
};

template <typename ValueType>
template <class PrimeGeneratorType, class RandomGeneratorType>
DiffieHellmanSecurityBase<ValueType> DiffieHellmanSecurityBase<ValueType>::create(PrimeGeneratorType& primeGenerator,
                                                                                  RandomGeneratorType& randomGenerator)
{
    static_assert(IsPrimeGenerator<PrimeGeneratorType>::value,
                  "Invalid template argument for cml::DiffieHellmanSecurityBase: PrimeGeneratorType "
                  "interface is not suitable");
    static_assert(IsRandomGenerator<RandomGeneratorType>::value,
                  "Invalid template argument for cml::DiffieHellmanSecurityBase::create(...): RandomGeneratorType "
                  "interface is not suitable");

    DiffieHellmanSecurityBase securityBase{};
    securityBase.p = static_cast<Value>(primeGenerator());
    securityBase.g = primitiveRootModulo(securityBase.p, randomGenerator);

    return securityBase;
}

template <typename ValueType>
struct DiffieHellmanPublicKey {
    using Value = ValueType;
    Value v{ 0 };
};

template <typename ValueType>
struct DiffieHellmanPrivateKey {
    using Value = ValueType;
    Value a{ 0 };
};

/* ================================================================================= */
/* ============================= DiffieHellmanProtocol ============================= */
/* ================================================================================= */

template <typename ValueType, class RandomGeneratorType>
struct DiffieHellmanProtocol {
    static_assert(IsRandomGenerator<RandomGeneratorType>::value,
                  "Invalid template argument for cml::DiffieHellmanProtocol: "
                  "RandomGeneratorType interface is not suitable");

    using Value           = ValueType;
    using PublicKey       = DiffieHellmanPublicKey<Value>;
    using PrivateKey      = DiffieHellmanPrivateKey<Value>;
    using SecurityBase    = DiffieHellmanSecurityBase<Value>;
    using RandomGenerator = RandomGeneratorType;

    DiffieHellmanProtocol();
    explicit DiffieHellmanProtocol(const RandomGenerator& randomGenerator);

    void generate(SecurityBase securityBase);

    PublicKey publicKey{};
    PrivateKey privateKey{};
    SecurityBase securityBase{};
    RandomGenerator randomGenerator{};
};

template <typename ValueType, class RandomGeneratorType>
DiffieHellmanProtocol<ValueType, RandomGeneratorType>::DiffieHellmanProtocol() = default;

template <typename ValueType, class RandomGeneratorType>
DiffieHellmanProtocol<ValueType, RandomGeneratorType>::DiffieHellmanProtocol(const RandomGenerator& randomGenerator) :
    randomGenerator(randomGenerator)
{}

template <typename ValueType, class RandomGeneratorType>
void DiffieHellmanProtocol<ValueType, RandomGeneratorType>::generate(SecurityBase securityBase)
{
    if (securityBase.g == 0 || securityBase.p == 0)
        throw std::logic_error{ "Security base is not generated" };

    privateKey.a = static_cast<typename PrivateKey::Value>(randomGenerator());
    publicKey.v  = modexp(securityBase.g, privateKey.a, securityBase.p);
}
} // namespace cml