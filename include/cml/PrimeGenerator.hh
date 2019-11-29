#pragma once

namespace mech {
namespace crypt {
class PrimeGenerator {
public:
    using Result = std::uint64_t;

    virtual ~PrimeGenerator() = default;

    Result operator()()
    {
        return generate();
    }

private:
    virtual Result generate() = 0;
};
} // namespace crypt
} // namespace mech
