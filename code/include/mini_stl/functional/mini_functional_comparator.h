#ifndef MINI_FUNCTIONAL_COMPARATOR_H
#define MINI_FUNCTIONAL_COMPARATOR_H

#include <utility>

namespace mini::func {

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

}  // namespace mini::func

#endif
