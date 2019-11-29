#include <array>
#include <chrono>
#include <cml/cml.hh>
#include <iostream>
#include <numeric>

int main()
{
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    std::chrono::time_point<std::chrono::high_resolution_clock> stop;

    using Timeholder                       = double;
    constexpr std::int32_t testRepeatCount = 1000;

    std::array<Timeholder, testRepeatCount> testDurations{};
    mech::crypt::MilRabPrimeGenerator primeGenerator;

    for (std::int32_t i = 0; i < testRepeatCount; ++i) {
        start = std::chrono::high_resolution_clock::now();

        auto prime = primeGenerator();
        (void)(prime);

        stop = std::chrono::high_resolution_clock::now();

        testDurations[i] = std::chrono::duration<Timeholder, std::milli>(stop - start).count();
    }

    auto averageDuration =
        std::accumulate(testDurations.begin(), testDurations.end(), 0) / static_cast<Timeholder>(testRepeatCount);

    std::cout << "Test repeat count:            " << testRepeatCount << std::endl
              << "Average generation time (ms): " << averageDuration << std::endl;

    /* nothing to do */
    return 0;
}