#include <array>
#include <chrono>
#include <cml/cml.hh>
#include <fbi/fbi.hh>
#include <iostream>
#include <numeric>

int main()
{
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    std::chrono::time_point<std::chrono::high_resolution_clock> stop;

    constexpr std::int32_t testRepeatCount = 100;
    constexpr std::uint32_t primeDimension = 64;

    std::array<long long, testRepeatCount> testDurations{};
    mech::crypt::MilRabPrimeGenerator<primeDimension, fbi::BigUnsigned> primeGenerator;

    using milliseconds = std::chrono::duration<long long, std::milli>;
    for (std::int32_t i = 0; i < testRepeatCount; ++i) {
        start = std::chrono::high_resolution_clock::now();

        auto prime = primeGenerator();
        (void)(prime);

        stop = std::chrono::high_resolution_clock::now();

        testDurations[i] = std::chrono::duration_cast<milliseconds>(stop - start).count();
    }

    auto averageDuration = std::accumulate(testDurations.begin(), testDurations.end(), 0) / testRepeatCount;

    std::cout << "Test repeat count:            " << testRepeatCount << std::endl
              << "Prime number dimension (bits) " << primeDimension << std::endl
              << "Average generation time (ms): " << averageDuration << std::endl;

    /* nothing to do */
    return 0;
}