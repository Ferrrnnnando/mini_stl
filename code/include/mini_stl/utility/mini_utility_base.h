#ifndef MINI_UTILITY_BASE_H
#define MINI_UTILITY_BASE_H

namespace mini::util {

template<typename T1, typename T2>
struct pair {
    constexpr pair(const T1& x, const T2& y)
        : first(x)
        , second(y)
    {}

    template<typename U1, typename U2>
    constexpr pair(U1&& x, U2&& y)
        : first(x)
        , second(y)
    {}

    T1 first;
    T2 second;
};

}  // namespace mini::util

#endif
