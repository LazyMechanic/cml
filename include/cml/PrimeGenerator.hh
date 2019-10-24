#pragma once

#include <cstdint>

#include "IsPowerOf2.hh"

namespace mech {
namespace crypt {
template <std::uint32_t primeDimension, typename ResultType>
class PrimeGenerator {
public:
    static_assert(IsPowerOf2<std::uint32_t, primeDimension>::value,
                  "Invalid template argument for mech::crypt::PrimeGenerator: primeDimension is not a power of 2");

    static constexpr std::uint32_t resultDimension = primeDimension;
    using Result                                   = ResultType;

    virtual ~PrimeGenerator() = default;

    Result operator()();

private:
    virtual Result generate() = 0;
};

template <std::uint32_t primeDimension, typename ResultType>
typename PrimeGenerator<primeDimension, ResultType>::Result PrimeGenerator<primeDimension, ResultType>::operator()()
{
    return generate();
}
} // namespace crypt
} // namespace mech
