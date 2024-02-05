#ifndef MIN_FUNCTIONAL_LOGICAL_H
#define MIN_FUNCTIONAL_LOGICAL_H

#include "mini_stl/functional/mini_functional_base.h"

namespace mini::func {

template<typename T>
struct logical_and : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x && y; }
};

template<typename T>
struct logical_or : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x || y; }
};

template<typename T>
struct logical_not : public unary_function<T, bool> {
    bool operator()(const T& x, const T& y) const { return !x; }
};

}  // namespace mini::func

#endif
