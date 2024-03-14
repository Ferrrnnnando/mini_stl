#ifndef MINI_FUNCTIONAL_ADAPTERS_H
#define MINI_FUNCTIONAL_ADAPTERS_H

#include "mini_stl/functional/mini_functional_base.h"

namespace mini::func {

template<typename Func>
class binder1st
    : public unary_function<typename Func::second_argument_type, typename Func::result_type> {
protected:
    Func op_;
    typename Func::first_argument_type first_value_;  // value binded

public:
    binder1st(const Func& func, const typename Func::first_argument_type& first_value)
        : op_(func)
        , first_value_(first_value)
    {}

    typename Func::result_type operator()(
        const typename Func::second_argument_type& second_value) const
    {
        return op_(first_value_, second_value);
    }
};

// helper function
template<typename Func, typename T>
binder1st<Func> bind1st(const Func& func, const T& value)
{
    return binder1st(func, value);
}

}  // namespace mini::func

#endif
