#pragma once

#include <random>

#include "RandomGenerator.hh"

namespace mech {
namespace crypt {
class Mt19937RandomGenerator : public RandomGenerator {
public:
    using Base   = RandomGenerator;
    using Engine = std::mt19937;
    using Base::Result;

    Mt19937RandomGenerator() = default;
    explicit Mt19937RandomGenerator(const Engine::result_type& seed) : m_randomEngine(seed) {}

    Result random() override
    {
        return m_randomEngine();
    }

    Result random(Result min, Result max) override
    {
        std::uniform_int_distribution<Result> uid{min, max};
        return uid(m_randomEngine);
    }

private:
    Engine m_randomEngine{ std::random_device{}() };
};
} // namespace crypt
} // namespace mech