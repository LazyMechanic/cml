#pragma once

#include <type_traits>

#include "Typedefs.hh"

namespace cml {

template <typename T>
struct ExtendedContainer {
    using Type = std::conditional_t<
        std::is_same<T, Uint8>::value,
        Uint16,
        std::conditional_t<
            std::is_same<T, Uint16>::value,
            Uint32,
            std::conditional_t<
                std::is_same<T, Uint32>::value,
                Uint64,
                std::conditional_t<
                    std::is_same<T, Uint64>::value,
                    Uint128,
                    std::conditional_t<
                        std::is_same<T, Uint128>::value,
                        Uint256,
                        std::conditional_t<
                            std::is_same<T, Uint256>::value,
                            Uint512,
                            std::conditional_t<
                                std::is_same<T, Uint512>::value,
                                Uint1024,
                                std::conditional_t<
                                    std::is_same<T, Int8>::value,
                                    Int16,
                                    std::conditional_t<
                                        std::is_same<T, Int16>::value,
                                        Int32,
                                        std::conditional_t<
                                            std::is_same<T, Int32>::value,
                                            Int64,
                                            std::conditional_t<
                                                std::is_same<T, Int64>::value,
                                                Int128,
                                                std::conditional_t<
                                                    std::is_same<T, Int128>::value,
                                                    Int256,
                                                    std::conditional_t<
                                                        std::is_same<T, Int256>::value,
                                                        Int512,
                                                        std::conditional_t<std::is_same<T, Int512>::value,
                                                                           Int1014,
                                                                           UnboundedInt>>>>>>>>>>>>>>;
};

} // namespace cml