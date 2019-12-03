#pragma once

#include <cstdint>
#include <type_traits>

#include <boost/multiprecision/cpp_int.hpp>

namespace cml {

template <std::uint32_t bitness, bool sign = false>
struct ContainerByBitness {
    using Type = std::conditional_t<
        sign,
        std::conditional_t<
            bitness <= 8,
            std::int8_t,
            std::conditional_t<
                bitness <= 16,
                std::int16_t,
                std::conditional_t<
                    bitness <= 32,
                    std::int32_t,
                    std::conditional_t<
                        bitness <= 64,
                        std::int64_t,
                        std::conditional_t<
                            bitness <= 128,
                            boost::multiprecision::int128_t,
                            std::conditional_t<
                                bitness <= 256,
                                boost::multiprecision::int256_t,
                                std::conditional_t<bitness <= 512,
                                                   boost::multiprecision::int512_t,
                                                   std::conditional_t<bitness <= 1024,
                                                                      boost::multiprecision::int1024_t,
                                                                      boost::multiprecision::cpp_int>>>>>>>>,
        std::conditional_t<
            bitness <= 8,
            std::uint8_t,
            std::conditional_t<
                bitness <= 16,
                std::uint16_t,
                std::conditional_t<
                    bitness <= 32,
                    std::uint32_t,
                    std::conditional_t<
                        bitness <= 64,
                        std::uint64_t,
                        std::conditional_t<
                            bitness <= 128,
                            boost::multiprecision::uint128_t,
                            std::conditional_t<
                                bitness <= 256,
                                boost::multiprecision::uint256_t,
                                std::conditional_t<bitness <= 512,
                                                   boost::multiprecision::uint512_t,
                                                   std::conditional_t<bitness <= 1024,
                                                                      boost::multiprecision::uint1024_t,
                                                                      boost::multiprecision::cpp_int>>>>>>>>>;
};

} // namespace cml