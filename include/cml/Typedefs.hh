#pragma once

#include <cstdint>

#include <boost/multiprecision/cpp_int.hpp>

namespace cml {

using Uint8    = std::uint8_t;
using Uint16   = std::uint16_t;
using Uint32   = std::uint32_t;
using Uint64   = std::uint64_t;
using Uint128  = boost::multiprecision::uint128_t;
using Uint256  = boost::multiprecision::uint256_t;
using Uint512  = boost::multiprecision::uint512_t;
using Uint1024 = boost::multiprecision::uint1024_t;

using Int8    = std::int8_t;
using Int16   = std::int16_t;
using Int32   = std::int32_t;
using Int64   = std::int64_t;
using Int128  = boost::multiprecision::int128_t;
using Int256  = boost::multiprecision::int256_t;
using Int512  = boost::multiprecision::int512_t;
using Int1014 = boost::multiprecision::int1024_t;

using UnboundedInt = boost::multiprecision::cpp_int;

} // namespace cml