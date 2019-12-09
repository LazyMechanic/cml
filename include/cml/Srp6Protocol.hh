#pragma once

#include <condition_variable>
#include <cstdint>
#include <functional>
#include <future>
#include <sstream>
#include <string>
#include <thread>

#include "Algorithms.hh"
#include "ExtendedContainer.hh"
#include "IsPrimeGenerator.hh"
#include "Typedefs.hh"
#include "picosha2.h"

namespace cml {

template <typename... Args>
Uint256 srp6Hash(Args&&... args)
{
    std::string toHash = (boost::lexical_cast<std::string>(args) + ... + "");
    std::string hash   = picosha2::hash256_hex_string(toHash);

    return Uint256{ addBasePrefix(hash) };
}

/* ================================================================================= */
/* =============================== Srp6SecurityBase ================================ */
/* ================================================================================= */

template <typename ValueType>
struct Srp6SecurityBase {
    using SafePrime          = ValueType;
    using MultGroupGenerator = ValueType;

    template <class SafePrimeGeneratorType, class RandomGeneratorType>
    static Srp6SecurityBase create(SafePrimeGeneratorType& safePrimeGenerator, RandomGeneratorType& randomGenerator);

    template <class SafePrimeGeneratorType, class RandomGeneratorType>
    static Srp6SecurityBase create();

    template <class SafePrimeGeneratorType, class RandomGeneratorType>
    static Srp6SecurityBase createA(SafePrimeGeneratorType& safePrimeGenerator, RandomGeneratorType& randomGenerator);

    template <class SafePrimeGeneratorType, class RandomGeneratorType>
    static Srp6SecurityBase createA();

    SafePrime N{}; // safe prime
    MultGroupGenerator g{}; // generator of the multiplicative group
    Uint256 k{};
};

template <typename ValueType>
template <class SafePrimeGeneratorType, class RandomGeneratorType>
Srp6SecurityBase<ValueType> Srp6SecurityBase<ValueType>::create(SafePrimeGeneratorType& safePrimeGenerator,
                                                                RandomGeneratorType& randomGenerator)
{
    static_assert(
        IsPrimeGenerator<SafePrimeGeneratorType>::value,
        "Invalid template argument for cml::Srp6SecurityBase::create(...): SafePrimeGeneratorType interface is "
        "not suitable");

    Srp6SecurityBase securityBase{};

    securityBase.N = static_cast<SafePrime>(safePrimeGenerator());
    securityBase.g = static_cast<MultGroupGenerator>(primitiveRootModulo(securityBase.N, randomGenerator));
    securityBase.k = 3;

    return securityBase;
}

template <typename ValueType>
template <class SafePrimeGeneratorType, class RandomGeneratorType>
Srp6SecurityBase<ValueType> Srp6SecurityBase<ValueType>::create()
{
    SafePrimeGeneratorType safePrimeGenerator{};
    RandomGeneratorType randomGenerator{};
    return create(safePrimeGenerator, randomGenerator);
}

template <typename ValueType>
template <class SafePrimeGeneratorType, class RandomGeneratorType>
Srp6SecurityBase<ValueType> Srp6SecurityBase<ValueType>::createA(SafePrimeGeneratorType& safePrimeGenerator,
                                                                 RandomGeneratorType& randomGenerator)
{
    static_assert(
        IsPrimeGenerator<SafePrimeGeneratorType>::value,
        "Invalid template argument for cml::Srp6SecurityBase::create(...): SafePrimeGeneratorType interface is "
        "not suitable");

    Srp6SecurityBase securityBase = create(safePrimeGenerator);

    securityBase.k = srp6Hash(securityBase.N, securityBase.g);
    return securityBase;
}

template <typename ValueType>
template <class SafePrimeGeneratorType, class RandomGeneratorType>
Srp6SecurityBase<ValueType> Srp6SecurityBase<ValueType>::createA()
{
    SafePrimeGeneratorType safePrimeGenerator{};
    RandomGeneratorType randomGenerator{};
    return createA(safePrimeGenerator, randomGenerator);
}

/* ================================================================================= */
/* ================================== Srp6Server =================================== */
/* ================================================================================= */

template <typename KeyType>
struct Srp6ServerPublicKey {
    using Key = KeyType;

    std::string salt;
    Key B;
};

struct Srp6ServerPrivateKey {
    using Hash = Uint256;
    Hash K;
};

template <typename VerifierType>
struct Srp6ServerData {
    using Hash     = Uint256;
    using Verifier = VerifierType;

    template <class RandomGeneratorType>
    static Srp6ServerData create(RandomGeneratorType& randomGenerator,
                                 const Srp6SecurityBase<VerifierType>& securityBase,
                                 const std::string& password);

    std::string salt;
    Hash x; // Hash of salt and password
    Verifier v; // Password verifier
};

template <typename VerifierType>
template <class RandomGeneratorType>
Srp6ServerData<VerifierType> Srp6ServerData<VerifierType>::create(RandomGeneratorType& randomGenerator,
                                                                  const Srp6SecurityBase<VerifierType>& securityBase,
                                                                  const std::string& password)
{
    Srp6ServerData data{};

    data.salt = randomString(password.size(), randomGenerator);
    data.x    = srp6Hash(data.salt, password);

    UnboundedInt g = securityBase.g;
    UnboundedInt x = data.x;
    UnboundedInt N = securityBase.N;

    data.v = static_cast<Verifier>(modexp(g, x, N));

    return data;
}

template <typename>
struct Srp6ClientPublicKey;

template <typename ValueType, class RandomGeneratorType>
struct Srp6Server {
    static_assert(IsRandomGenerator<RandomGeneratorType>::value,
                  "Invalid template argument for cml::Srp6Server: "
                  "RandomGeneratorType interface is not suitable");

    using Value           = ValueType;
    using Hash            = Uint256;
    using Data            = Srp6ServerData<Value>;
    using PublicKey       = Srp6ServerPublicKey<Value>;
    using PrivateKey      = Srp6ServerPrivateKey;
    using ClientPublicKey = Srp6ClientPublicKey<Value>;
    using SecurityBase    = Srp6SecurityBase<Value>;
    using RandomGenerator = RandomGeneratorType;

    Srp6Server();
    explicit Srp6Server(const RandomGenerator& randomGenerator);

    PublicKey generatePublicKey(const SecurityBase& securityBase, const Value& b, const Data& data);
    PrivateKey generatePrivateKey(const SecurityBase& securityBase,
                                  const Data& data,
                                  const Value& b,
                                  const PublicKey& serverPublicKey,
                                  const ClientPublicKey& clientPublicKey);

    RandomGenerator randomGenerator{};
};

template <typename ValueType, class RandomGeneratorType>
Srp6Server<ValueType, RandomGeneratorType>::Srp6Server() = default;

template <typename ValueType, class RandomGeneratorType>
Srp6Server<ValueType, RandomGeneratorType>::Srp6Server(const RandomGenerator& randomGenerator) :
    randomGenerator(randomGenerator)
{}

template <typename ValueType, class RandomGeneratorType>
typename Srp6Server<ValueType, RandomGeneratorType>::PublicKey
    Srp6Server<ValueType, RandomGeneratorType>::generatePublicKey(const SecurityBase& securityBase,
                                                                  const Value& b,
                                                                  const Data& data)
{
    PublicKey publicKey{};

    publicKey.salt = data.salt;

    using ModExpType = UnboundedInt;

    UnboundedInt kv  = UnboundedInt{ securityBase.k } * data.v;
    UnboundedInt gb  = modexp<ModExpType>(securityBase.g, b, securityBase.N);
    UnboundedInt res = kv % securityBase.N + gb;

    publicKey.B = static_cast<Value>(res);

    return publicKey;
}

template <typename ValueType, class RandomGeneratorType>
typename Srp6Server<ValueType, RandomGeneratorType>::PrivateKey
    Srp6Server<ValueType, RandomGeneratorType>::generatePrivateKey(const SecurityBase& securityBase,
                                                                   const Data& data,
                                                                   const Value& b,
                                                                   const PublicKey& serverPublicKey,
                                                                   const ClientPublicKey& clientPublicKey)
{
    UnboundedInt u = srp6Hash(clientPublicKey.A, serverPublicKey.B);
    UnboundedInt A = clientPublicKey.A;
    UnboundedInt N = securityBase.N;
    UnboundedInt v = data.v;

    UnboundedInt base    = A * modexp(v, u, N);
    UnboundedInt exp     = b;
    UnboundedInt modulus = securityBase.N;

    UnboundedInt S = modexp(base, exp, modulus);

    PrivateKey privateKey{};
    privateKey.K = srp6Hash(S);

    return privateKey;
}

/* ================================================================================= */
/* ================================== Srp6Client =================================== */
/* ================================================================================= */

template <typename KeyType>
struct Srp6ClientPublicKey {
    using Key = KeyType;

    std::string identifier;
    Key A;
};

struct Srp6ClientPrivateKey {
    using Hash = Uint256;
    Hash K;
};

template <typename ValueType, class RandomGeneratorType>
struct Srp6Client {
    static_assert(IsRandomGenerator<RandomGeneratorType>::value,
                  "Invalid template argument for cml::Srp6Client: "
                  "RandomGeneratorType interface is not suitable");

    using Value           = ValueType;
    using SecurityBase    = Srp6SecurityBase<Value>;
    using PublicKey       = Srp6ClientPublicKey<Value>;
    using PrivateKey      = Srp6ClientPrivateKey;
    using ServerPublicKey = Srp6ServerPublicKey<Value>;
    using RandomGenerator = RandomGeneratorType;

    Srp6Client();
    explicit Srp6Client(const RandomGenerator& randomGenerator);

    PublicKey generatePublicKey(const SecurityBase& securityBase, const Value& a, const std::string& identifier);
    PrivateKey generatePrivateKey(const SecurityBase& securityBase,
                                  const std::string& password,
                                  const Value& a,
                                  const PublicKey& clientPublicKey,
                                  const ServerPublicKey& serverPublicKey);

    RandomGenerator randomGenerator{};
};

template <typename ValueType, class RandomGeneratorType>
Srp6Client<ValueType, RandomGeneratorType>::Srp6Client() = default;

template <typename ValueType, class RandomGeneratorType>
Srp6Client<ValueType, RandomGeneratorType>::Srp6Client(const RandomGenerator& randomGenerator) :
    randomGenerator(randomGenerator)
{}

template <typename ValueType, class RandomGeneratorType>
typename Srp6Client<ValueType, RandomGeneratorType>::PublicKey
    Srp6Client<ValueType, RandomGeneratorType>::generatePublicKey(const SecurityBase& securityBase,
                                                                  const Value& a,
                                                                  const std::string& identifier)
{
    PublicKey publicKey{};

    publicKey.identifier = identifier;

    using ModExpType = UnboundedInt;
    publicKey.A      = static_cast<Value>(modexp<ModExpType>(securityBase.g, a, securityBase.N));

    return publicKey;
}

template <typename ValueType, class RandomGeneratorType>
typename Srp6Client<ValueType, RandomGeneratorType>::PrivateKey
    Srp6Client<ValueType, RandomGeneratorType>::generatePrivateKey(const SecurityBase& securityBase,
                                                                   const std::string& password,
                                                                   const Value& a,
                                                                   const PublicKey& clientPublicKey,
                                                                   const ServerPublicKey& serverPublicKey)
{
    UnboundedInt x = srp6Hash(serverPublicKey.salt, password);
    UnboundedInt u = srp6Hash(clientPublicKey.A, serverPublicKey.B);

    UnboundedInt N = securityBase.N;
    UnboundedInt g = securityBase.g;
    UnboundedInt k = securityBase.k;
    UnboundedInt B = serverPublicKey.B;

    UnboundedInt base    = B - (k * modexp(g, x, N)) % N;
    UnboundedInt exp     = u * x + a;
    UnboundedInt modulus = securityBase.N;

    UnboundedInt S = modexp(base, exp, modulus);

    PrivateKey privateKey{};
    privateKey.K = srp6Hash(S);

    return privateKey;
}
} // namespace cml
