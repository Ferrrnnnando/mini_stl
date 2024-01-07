#ifndef MINI_FUNCTIONAL_BASE_H
#define MINI_FUNCTIONAL_BASE_H

#include <cstring>

namespace mini::func {

template<typename T>
struct equal_to {
    bool operator()(const T& first, const T& second) { return first == second; }
};

// template for const pointer
template<typename T>
struct equal_to<const T*> {
    bool operator()(const T* first, const T* second) { return first == second; }
};

template<>
struct equal_to<const char*> {
    bool operator()(const char* first, const char* second) { return strcmp(first, second) == 0; }
};

struct identity {
    template<typename T>
    constexpr T&& operator()(T&& t) const
    {
        return std::forward<T>(t);
    }
};

struct select1st {
    template<typename T>
    constexpr auto operator()(T&& t) const -> decltype(auto)
    {
        return std::forward<decltype(t.first)>(t.first);
    }
};

}  // namespace mini::func

#endif
