#pragma once

#include <boost/multiprecision/cpp_int.hpp>

#include "Algorithms.hh"
#include "IsPrimeGenerator.hh"
#include "Typedefs.hh"

namespace cml {

struct RsaPublicKey {
    UnboundedInt e{ 0 };
    UnboundedInt n{ 0 };
};

struct RsaPrivateKey {
    UnboundedInt d{ 0 };
    UnboundedInt n{ 0 };
};

template <typename PrimeGeneratorType>
struct RsaProtocol {
    static_assert(IsPrimeGenerator<PrimeGeneratorType>::value,
                  "Invalid template argument for cml::RsaProtocol: PrimeGeneratorType "
                  "interface is not suitable");

    using PublicKey      = RsaPublicKey;
    using PrivateKey     = RsaPrivateKey;
    using PrimeGenerator = PrimeGeneratorType;

    RsaProtocol();
    explicit RsaProtocol(const PrimeGenerator& primeGenerator);

    void generate();

    UnboundedInt encrypt(const Uint64& source, const PublicKey& anotherPublicKey);
    std::vector<UnboundedInt> encrypt(const std::vector<Uint64>& source, const PublicKey& anotherPublicKey);

    Uint64 decrypt(const UnboundedInt& source);
    std::vector<Uint64> decrypt(const std::vector<UnboundedInt>& source);

    PublicKey publicKey{};
    PrivateKey privateKey{};
    PrimeGenerator primeGenerator{};
};

template <typename PrimeGeneratorType>
RsaProtocol<PrimeGeneratorType>::RsaProtocol() = default;

template <typename PrimeGeneratorType>
RsaProtocol<PrimeGeneratorType>::RsaProtocol(const PrimeGenerator& primeGenerator) : primeGenerator(primeGenerator)
{}

template <typename PrimeGeneratorType>
void RsaProtocol<PrimeGeneratorType>::generate()
{
    UnboundedInt p{ primeGenerator() };
    UnboundedInt q{ primeGenerator() };

    // Compute phi
    UnboundedInt phi = (p - 1) * (q - 1);

    // Compute 'n'
    UnboundedInt n = p * q;
    publicKey.n    = n;
    privateKey.n   = n;

    // Mersenne prime number
    publicKey.e = 65537;

    // Compute 'd'
    privateKey.d = invmod(publicKey.e, phi);
}

template <typename PrimeGeneratorType>
UnboundedInt RsaProtocol<PrimeGeneratorType>::encrypt(const Uint64& source, const PublicKey& anotherPublicKey)
{
    return modexp<UnboundedInt>(UnboundedInt{ source }, anotherPublicKey.e, anotherPublicKey.n);
}

template <typename PrimeGeneratorType>
std::vector<UnboundedInt> RsaProtocol<PrimeGeneratorType>::encrypt(const std::vector<Uint64>& source,
                                                                   const PublicKey& anotherPublicKey)
{
    std::vector<UnboundedInt> result{};
    result.resize(source.size());

    for (std::size_t i = 0; i < source.size(); ++i) {
        result[i] = encrypt(source[i], anotherPublicKey);
    }

    return result;
}

template <typename PrimeGeneratorType>
Uint64 RsaProtocol<PrimeGeneratorType>::decrypt(const UnboundedInt& source)
{
    return static_cast<Uint64>(modexp<UnboundedInt>(source, privateKey.d, privateKey.n));
}

template <typename PrimeGeneratorType>
std::vector<Uint64> RsaProtocol<PrimeGeneratorType>::decrypt(const std::vector<UnboundedInt>& source)
{
    std::vector<Uint64> result{};
    result.resize(source.size());

    for (std::size_t i = 0; i < source.size(); ++i) {
        result[i] = decrypt(source[i]);
    }

    return result;
}

} // namespace cml