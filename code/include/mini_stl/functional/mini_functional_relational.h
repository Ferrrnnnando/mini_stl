#ifndef MIN_FUNCTIONAL_RELATIONAL_H
#define MIN_FUNCTIONAL_RELATIONAL_H

#include "mini_stl/functional/mini_functional_base.h"

namespace mini::func {

template<typename T>
struct equal_to : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x == y; }
};

// template for const pointer
template<typename T>
struct equal_to<const T*> {
    bool operator()(const T* first, const T* second) const { return first == second; }
};

template<>
struct equal_to<const char*> {
    bool operator()(const char* first, const char* second) const
    {
        return strcmp(first, second) == 0;
    }
};

template<typename T>
struct not_equal_to : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x != y; }
};

template<typename T>
struct greater : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x > y; }
};

template<typename T>
struct greater_equal : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x >= y; }
};

/**
 * @brief Function objects for performing comparisons.
 *
 * @tparam T
 */
template<typename T = void>
struct less {
    constexpr bool operator()(const T& lhs, const T& rhs) const { return lhs < rhs; }
};

// A 'void' specialization of less, with parameter and return type deduced
template<>
struct less<void> {
    template<typename T, typename U>
    constexpr auto operator()(T&& lhs, U&& rhs) const
        -> decltype(std::forward<T>(lhs) < std::forward<U>(rhs))
    {
        return std::forward<T>(lhs) < std::forward<U>(rhs);
    }
};

template<typename T>
struct less_equal : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x <= y; }
};

}  // namespace mini::func

#endif
