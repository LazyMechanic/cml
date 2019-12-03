#pragma once

#include <boost/multiprecision/cpp_int.hpp>

#include "Algorithms.hh"
#include "IsPrimeGenerator.hh"

namespace cml {

struct RsaPublicKey {
    boost::multiprecision::cpp_int e{ 0 };
    boost::multiprecision::cpp_int n{ 0 };
};

struct RsaPrivateKey {
    boost::multiprecision::cpp_int d{ 0 };
    boost::multiprecision::cpp_int n{ 0 };
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

    boost::multiprecision::cpp_int encrypt(const std::uint64_t& source, const PublicKey& anotherPublicKey);
    std::vector<boost::multiprecision::cpp_int> encrypt(const std::vector<std::uint64_t>& source,
                                                        const PublicKey& anotherPublicKey);

    std::uint64_t decrypt(const boost::multiprecision::cpp_int& source);
    std::vector<std::uint64_t> decrypt(const std::vector<boost::multiprecision::cpp_int>& source);

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
    boost::multiprecision::cpp_int p{ primeGenerator() };
    boost::multiprecision::cpp_int q{ primeGenerator() };

    // Compute phi
    boost::multiprecision::cpp_int phi = (p - 1) * (q - 1);

    // Compute 'n'
    boost::multiprecision::cpp_int n = p * q;
    publicKey.n                      = n;
    privateKey.n                     = n;

    // Mersenne prime number
    publicKey.e = 65537;

    // Compute 'd'
    privateKey.d = invmod(publicKey.e, phi);
}

template <typename PrimeGeneratorType>
boost::multiprecision::cpp_int RsaProtocol<PrimeGeneratorType>::encrypt(const std::uint64_t& source,
                                                                        const PublicKey& anotherPublicKey)
{
    return modexp<boost::multiprecision::cpp_int>(
        boost::multiprecision::cpp_int{ source }, anotherPublicKey.e, anotherPublicKey.n);
}

template <typename PrimeGeneratorType>
std::vector<boost::multiprecision::cpp_int>
    RsaProtocol<PrimeGeneratorType>::encrypt(const std::vector<std::uint64_t>& source,
                                             const PublicKey& anotherPublicKey)
{
    std::vector<boost::multiprecision::cpp_int> result{};
    result.resize(source.size());

    for (std::size_t i = 0; i < source.size(); ++i) {
        result[i] = encrypt(source[i], anotherPublicKey);
    }

    return result;
}

template <typename PrimeGeneratorType>
std::uint64_t RsaProtocol<PrimeGeneratorType>::decrypt(const boost::multiprecision::cpp_int& source)
{
    return static_cast<std::uint64_t>(modexp<boost::multiprecision::cpp_int>(source, privateKey.d, privateKey.n));
}

template <typename PrimeGeneratorType>
std::vector<std::uint64_t>
    RsaProtocol<PrimeGeneratorType>::decrypt(const std::vector<boost::multiprecision::cpp_int>& source)
{
    std::vector<std::uint64_t> result{};
    result.resize(source.size());

    for (std::size_t i = 0; i < source.size(); ++i) {
        result[i] = decrypt(source[i]);
    }

    return result;
}

} // namespace cml