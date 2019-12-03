#pragma once

#include <mutex>
#include <stdexcept>

#include "LaunchPolicy.hh"

namespace cml {

template <typename ResultType>
class RandomGenerator {
public:
    using Result = ResultType;

    virtual ~RandomGenerator() = default;

    Result operator()(LaunchPolicy policy = LaunchPolicy::Sync)
    {
        switch (policy) {
            case LaunchPolicy::Async: {
                std::unique_lock<std::mutex> lock{ m_mutex };
                return random();
            }
            case LaunchPolicy::Sync:
                return random();
            default:
                throw std::domain_error{ "cml::RandomGenerator::operator()(policy): Invalid policy" };
        }

        return Result{};
    }

    Result operator()(Result min, Result max, LaunchPolicy policy = LaunchPolicy::Sync)
    {
        switch (policy) {
            case LaunchPolicy::Async: {
                std::unique_lock<std::mutex> lock{ m_mutex };
                return random(min, max);
            }
            case LaunchPolicy::Sync:
                return random(min, max);
            default:
                throw std::domain_error{ "cml::RandomGenerator::operator()(..., policy): Invalid policy" };
        }

        return Result{};
    }

private:
    virtual Result random()                       = 0;
    virtual Result random(Result min, Result max) = 0;
    std::mutex m_mutex{};
};

} // namespace cml