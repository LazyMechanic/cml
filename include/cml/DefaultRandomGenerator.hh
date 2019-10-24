#pragma once

#include <random>

#include "IsPowerOf2.hh"
#include "RandomGenerator.hh"

namespace mech {
namespace crypt {
template <std::uint32_t numberDimension, typename ResultType>
struct DefaultRandomGenerator : RandomGenerator<ResultType> {
    static_assert(
        IsPowerOf2<std::uint32_t, numberDimension>::value,
        "Invalid template argument for mech::crypt::DefaultRandomGenerator: primeDimension is not a power of 2");

    static constexpr std::uint32_t resultDimension = numberDimension;
    using Base                                     = RandomGenerator<ResultType>;
    using Result                                   = typename Base::Result;
    using Engine = std::conditional_t<(resultDimension > 32), std::mt19937_64, std::mt19937>;

    Result random() override;

    Result random(Result min, Result max) override;

    Engine randomEngine{ std::random_device{}() };
};

template <std::uint32_t numberDimension, typename ResultType>
typename DefaultRandomGenerator<numberDimension, ResultType>::Result
    DefaultRandomGenerator<numberDimension, ResultType>::random()
{
    Result result{ randomEngine() };
    constexpr std::uint32_t shiftLength = resultDimension > 32 ? 64 : 32;
    for (std::uint32_t i = shiftLength; i < numberDimension; i += shiftLength) {
        result = (result << shiftLength) | randomEngine();
    }

    return result;
}

template <std::uint32_t numberDimension, typename ResultType>
typename DefaultRandomGenerator<numberDimension, ResultType>::Result
    DefaultRandomGenerator<numberDimension, ResultType>::random(Result min, Result max)
{
    Result result{ random() };
    result = result % (max - min) + min;
    return result;
}
} // namespace crypt
} // namespace mech