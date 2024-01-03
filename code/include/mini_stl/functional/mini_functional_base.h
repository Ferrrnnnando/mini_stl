#ifndef MINI_FUNCTIONAL_BASE_H
#define MINI_FUNCTIONAL_BASE_H

namespace mini::func {

struct identity {
    template<typename T>
    constexpr T&& operator()(T&& t) const
    {
        return std::forward<T>(t);
    }
};

}  // namespace mini::func

#endif
