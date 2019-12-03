#pragma once
#include <type_traits>

namespace cml {

template <typename T>
struct IsRandomGenerator {
private:
    template <typename... Args>
    using yes = std::true_type;

    using no = std::false_type;

    template <typename U>
    static constexpr auto check(U*)
        -> yes<typename U::Result,
               decltype(static_cast<typename U::Result (U::*)(LaunchPolicy)>(&U::operator())),
               decltype(static_cast<typename U::Result (U::*)(typename U::Result, typename U::Result, LaunchPolicy)>(
                   &U::operator()))>;

    template <typename U>
    static constexpr no check(...);

    using type = decltype(check<T>(nullptr));

public:
    static constexpr bool value = type::value;
};

} // namespace cml