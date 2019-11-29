#pragma once

namespace mech {
namespace crypt {
template <typename PublicKeyType>
class PrivateKeyGenerator {
public:
    using Result    = std::uint64_t;
    using PublicKey = PublicKeyType;

    virtual ~PrivateKeyGenerator() = default;

    Result getPrivateKey() const
    {
        return m_privateKey;
    }

    PublicKey getPublicKey() const
    {
        return m_publicKey;
    }

    Result operator()(PublicKey key)
    {
        m_privateKey = generate(key);
        return getPrivateKey();
    }

private:
    Result m_privateKey{};
    PublicKey m_publicKey{};

    virtual Result generate(PublicKey key) = 0;
};
} // namespace crypt
} // namespace mech