#pragma once

#include <type_traits>

#include "Typedefs.hh"

namespace cml {

template <std::uint32_t bitness, bool sign = false>
struct ContainerByBitness {
    using Type = std::conditional_t<
        sign,
        std::conditional_t<
            bitness <= 8,
            Int8,
            std::conditional_t<
                bitness <= 16,
                Int16,
                std::conditional_t<
                    bitness <= 32,
                    Int32,
                    std::conditional_t<
                        bitness <= 64,
                        Int64,
                        std::conditional_t<
                            bitness <= 128,
                            Int128,
                            std::conditional_t<
                                bitness <= 256,
                                Int256,
                                std::conditional_t<bitness <= 512,
                                                   Int512,
                                                   std::conditional_t<bitness <= 1024, Int1014, UnboundedInt>>>>>>>>,
        std::conditional_t<
            bitness <= 8,
            Uint8,
            std::conditional_t<
                bitness <= 16,
                Uint16,
                std::conditional_t<
                    bitness <= 32,
                    Uint32,
                    std::conditional_t<
                        bitness <= 64,
                        Uint64,
                        std::conditional_t<
                            bitness <= 128,
                            Uint128,
                            std::conditional_t<
                                bitness <= 256,
                                Uint256,
                                std::conditional_t<
                                    bitness <= 512,
                                    Uint512,
                                    std::conditional_t<bitness <= 1024, Uint1024, UnboundedInt>>>>>>>>>;
};

} // namespace cml