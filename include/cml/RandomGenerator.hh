#pragma once

#include <future>
#include <mutex>

namespace mech {
namespace crypt {
class RandomGenerator {
public:
    using Result = std::uint64_t;

    virtual ~RandomGenerator() = default;

    Result operator()()
    {
        return random();
    }

    Result operator()(Result min, Result max)
    {
        return random(min, max);
    }

private:
    virtual Result random()                       = 0;
    virtual Result random(Result min, Result max) = 0;
};

} // namespace crypt
} // namespace mech