#pragma once

#include "Algorithms.hh"
#include "ExtendedContainer.hh"
#include "IsPrimeGenerator.hh"
#include "IsRandomGenerator.hh"
#include "PrimeGenerator.hh"

namespace cml {

template <class PrimeGeneratorType,
          class RandomGeneratorType,
          typename ResultType = typename ExtendedContainer<typename PrimeGeneratorType::Result>::Type>
class MilRabSafePrimeGenerator : public PrimeGenerator<ResultType> {
public:
    static_assert(IsPrimeGenerator<PrimeGeneratorType>::value,
                  "Invalid template argument for cml::MilRabSafePrimeGenerator: PrimeGeneratorType interface is "
                  "not suitable");

    static_assert(IsRandomGenerator<RandomGeneratorType>::value,
                  "Invalid template argument for cml::MilRabSafePrimeGenerator: RandomGeneratorType interface is "
                  "not suitable");

    using Base            = PrimeGenerator<ResultType>;
    using PrimeGenerator  = PrimeGeneratorType;
    using RandomGenerator = RandomGeneratorType;
    using Result          = typename Base::Result;

    MilRabSafePrimeGenerator();
    explicit MilRabSafePrimeGenerator(const PrimeGenerator& primeGenerator, const RandomGenerator& randomGenerator);

    Result generate() override;

    PrimeGenerator primeGenerator{};
    RandomGenerator randomGenerator{};
};

template <class PrimeGeneratorType, class RandomGeneratorType, typename ResultType>
MilRabSafePrimeGenerator<PrimeGeneratorType, RandomGeneratorType, ResultType>::MilRabSafePrimeGenerator() = default;

template <class PrimeGeneratorType, class RandomGeneratorType, typename ResultType>
MilRabSafePrimeGenerator<PrimeGeneratorType, RandomGeneratorType, ResultType>::MilRabSafePrimeGenerator(
    const PrimeGenerator& primeGenerator,
    const RandomGenerator& randomGenerator) :
    primeGenerator(primeGenerator), randomGenerator(randomGenerator)
{}

template <class PrimeGeneratorType, class RandomGeneratorType, typename ResultType>
typename MilRabSafePrimeGenerator<PrimeGeneratorType, RandomGeneratorType, ResultType>::Result
    MilRabSafePrimeGenerator<PrimeGeneratorType, RandomGeneratorType, ResultType>::generate()
{
    using SafePrime = Result;

    SafePrime safePrime{};
    while (true) {
        SafePrime prime = static_cast<SafePrime>(primeGenerator());
        safePrime       = prime * 2 + 1;

        auto testable      = prime;
        Uint32 repeatCount = 0;

        while (testable != 0) {
            testable = testable >> 1;
            ++repeatCount;
        }

        if (millerRabinTest(safePrime, repeatCount, randomGenerator))
            break;
    }

    return safePrime;
}
} // namespace cml