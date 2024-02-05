#ifndef MIN_FUNCTIONAL_ARITHMETIC_H
#define MIN_FUNCTIONAL_ARITHMETIC_H

#include "mini_stl/functional/mini_functional_base.h"

namespace mini::func {

template<typename T>
struct plus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x + y; }
};

template<typename T>
struct minus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x - y; }
};

}  // namespace mini::func

#endif
