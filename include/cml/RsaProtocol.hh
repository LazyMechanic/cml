#pragma once

#include <fbi/fbi.hh>

#include "Algorithms.hh"
#include "IsPrimeGenerator.hh"

namespace mech {
namespace crypt {
struct RsaPublicKey {
    fbi::BigInteger e{ 0 };
    fbi::BigInteger n{ 0 };
};

struct RsaPrivateKey {
    fbi::BigInteger d{ 0 };
    fbi::BigInteger n{ 0 };
};

template <typename PrimeGeneratorType = MilRabPrimeGenerator<50>>
struct RsaProtocol {
    static_assert(IsPrimeGenerator<PrimeGeneratorType>::value,
                  "Invalid template argument for mech::crypt::RsaProtocol: PrimeGeneratorType "
                  "interface is not suitable");

    using PublicKey      = RsaPublicKey;
    using PrivateKey     = RsaPrivateKey;
    using PrimeGenerator = PrimeGeneratorType;

    RsaProtocol();
    explicit RsaProtocol(const PrimeGenerator& primeGenerator);

    RsaProtocol& generate();

    fbi::BigInteger encrypt(const std::uint64_t& source, const PublicKey& anotherPublicKey);
    std::vector<fbi::BigInteger> encrypt(const std::vector<std::uint64_t>& source, const PublicKey& anotherPublicKey);

    std::uint64_t decrypt(const fbi::BigInteger& source);
    std::vector<std::uint64_t> decrypt(const std::vector<fbi::BigInteger>& source);

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
RsaProtocol<PrimeGeneratorType>& RsaProtocol<PrimeGeneratorType>::generate()
{
    fbi::BigInteger p{ primeGenerator() };
    fbi::BigInteger q{ primeGenerator() };

    // Compute phi
    fbi::BigInteger phi = (p - 1) * (q - 1);

    // Compute 'n'
    fbi::BigInteger n = p * q;
    publicKey.n       = n;
    privateKey.n      = n;

    // Mersenne prime number
    publicKey.e = 65537;

    // Compute 'd'
    privateKey.d = invmod(publicKey.e, phi);

    return *this;
}

template <typename PrimeGeneratorType>
fbi::BigInteger RsaProtocol<PrimeGeneratorType>::encrypt(const std::uint64_t& source, const PublicKey& anotherPublicKey)
{
    return modexp<fbi::BigUnsigned>(
        fbi::BigUnsigned{ source }, anotherPublicKey.e.getMagnitude(), anotherPublicKey.n.getMagnitude());
}

template <typename PrimeGeneratorType>
std::vector<fbi::BigInteger> RsaProtocol<PrimeGeneratorType>::encrypt(const std::vector<std::uint64_t>& source,
                                                                      const PublicKey& anotherPublicKey)
{
    std::vector<fbi::BigInteger> result{};
    result.resize(source.size());

    for (std::size_t i = 0; i < source.size(); ++i) {
        result[i] = encrypt(source[i], anotherPublicKey);
    }

    return result;
}

template <typename PrimeGeneratorType>
std::uint64_t RsaProtocol<PrimeGeneratorType>::decrypt(const fbi::BigInteger& source)
{
    return modexp<fbi::BigUnsigned>(source.getMagnitude(), privateKey.d.getMagnitude(), privateKey.n.getMagnitude())
        .toUnsignedLongLong();
}

template <typename PrimeGeneratorType>
std::vector<std::uint64_t> RsaProtocol<PrimeGeneratorType>::decrypt(const std::vector<fbi::BigInteger>& source)
{
    std::vector<std::uint64_t> result{};
    result.resize(source.size());

    for (std::size_t i = 0; i < source.size(); ++i) {
        result[i] = decrypt(source[i]);
    }

    return result;
}
} // namespace crypt
} // namespace mech