#pragma once

#include <random>

#include <boost/random.hpp>
#include <boost/random/random_device.hpp>

#include "ContainerByBitness.hh"
#include "RandomGenerator.hh"
#include "Typedefs.hh"

namespace cml {

template <Uint32 numberBitness, typename ResultType = typename ContainerByBitness<numberBitness>::Type>
class Mt19937RandomGenerator : public RandomGenerator<ResultType> {
public:
    static constexpr Uint32 bitness = numberBitness;

    using Base = RandomGenerator<ResultType>;
    using typename Base::Result;
    using Engine = boost::random::independent_bits_engine<std::mt19937, bitness, Result>;

    Mt19937RandomGenerator() = default;
    explicit Mt19937RandomGenerator(const typename Engine::result_type& seed) : m_randomEngine(seed) {}

    Result random() override
    {
        return m_randomEngine();
    }

    Result random(Result min, Result max) override
    {
        boost::random::uniform_int_distribution<Result> uid{ min, max };
        return uid(m_randomEngine);
    }

private:
    Engine m_randomEngine{ boost::random::random_device{}() };
};

} // namespace cml