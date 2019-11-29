#pragma once

#include <atomic>
#include <condition_variable>
#include <future>
#include <thread>
#include <tuple>
#include <vector>

#include "Algorithms.hh"
#include "IsPrimeGenerator.hh"
#include "IsRandomGenerator.hh"
#include "MilRabPrimeGenerator.hh"
#include "PrivateKeyGenerator.hh"

namespace mech {
namespace crypt {

/* ================================================================================= */
/* ============================ DiffieHellmanPublicKey ============================= */
/* ================================================================================= */

struct DiffieHellmanPublicKey {
    using Result = std::uint64_t;

    DiffieHellmanPublicKey();

    template <class PrimeGeneratorType>
    explicit DiffieHellmanPublicKey(PrimeGeneratorType primeGenerator);

    Result g;
    Result p;
    //
    // private:
    //    using FutureValueType = std::tuple<Result, Result>;
    //
    //    const unsigned m_threadAmount = std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency()
    //    : 2; std::vector<std::thread> m_threads{ m_threadAmount }; std::vector<std::shared_future<FutureValueType>>
    //    m_futures{ m_threadAmount };
};

inline DiffieHellmanPublicKey::DiffieHellmanPublicKey() = default;

template <class PrimeGeneratorType>
DiffieHellmanPublicKey::DiffieHellmanPublicKey(PrimeGeneratorType primeGenerator)
{
    static_assert(IsPrimeGenerator<PrimeGeneratorType>::value,
                  "Invalid template argument for mech::crypt::DiffieHellmanPublicKey::create(...): PrimeGeneratorType "
                  "interface is not suitable");

    this->p = primeGenerator();
    this->g = primitiveRootModulo(this->p);
}

/* ================================================================================= */
/* ============================= DiffieHellmanProtocol ============================= */
/* ================================================================================= */

template <class RandomGeneratorType = Mt19937RandomGenerator>
class DiffieHellmanProtocol {
public:
    static_assert(IsRandomGenerator<RandomGeneratorType>::value,
                  "Invalid template argument for mech::crypt::DiffieHellmanProtocol: RandomGeneratorType interface is "
                  "not suitable");

    using Result          = std::uint64_t;
    using PublicKey       = DiffieHellmanPublicKey;
    using RandomGenerator = RandomGeneratorType;

    DiffieHellmanProtocol();
    explicit DiffieHellmanProtocol(const RandomGenerator& randomGenerator);

    Result generate(PublicKey key);
    Result getA() const;

private:
    Result m_a{};
    PublicKey m_publicKey{};
    RandomGenerator m_randomGenerator{};
};

template <class RandomGeneratorType>
DiffieHellmanProtocol<RandomGeneratorType>::DiffieHellmanProtocol() = default;

template <class RandomGeneratorType>
DiffieHellmanProtocol<RandomGeneratorType>::DiffieHellmanProtocol(const RandomGenerator& randomGenerator) :
    m_randomGenerator(randomGenerator)
{}

template <class RandomGeneratorType>
typename DiffieHellmanProtocol<RandomGeneratorType>::Result
    DiffieHellmanProtocol<RandomGeneratorType>::generate(PublicKey key)
{
    m_a = m_randomGenerator();
    return modexp(key.g, m_a, key.p);
}

template <class RandomGeneratorType>
typename DiffieHellmanProtocol<RandomGeneratorType>::Result DiffieHellmanProtocol<RandomGeneratorType>::getA() const
{
    return m_a;
}
} // namespace crypt
} // namespace mech