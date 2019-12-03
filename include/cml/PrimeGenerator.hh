#pragma once

#include <mutex>
#include <stdexcept>

#include "LaunchPolicy.hh"

namespace cml {

template <typename ResultType>
class PrimeGenerator {
public:
    using Result = ResultType;

    virtual ~PrimeGenerator() = default;

    Result operator()(LaunchPolicy policy = LaunchPolicy::Sync)
    {
        switch (policy) {
            case LaunchPolicy::Async: {
                std::unique_lock<std::mutex> lock{ m_mutex };
                return generate();
            }
            case LaunchPolicy::Sync:
                return generate();
            default:
                throw std::domain_error{ "cml::PrimeGenerator::operator()(policy): Invalid policy" };
        }

        return Result{};
    }

private:
    virtual Result generate() = 0;
    std::mutex m_mutex{};
};

} // namespace cml
