#pragma once

namespace mech {
namespace crypt {
template <typename ResultType>
struct RandomGenerator {
public:
    using Result = ResultType;

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