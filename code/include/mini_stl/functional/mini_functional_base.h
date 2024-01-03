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

struct select1st {
    template<typename T>
    constexpr auto operator()(T&& t) const -> decltype(auto)
    {
        return std::forward<decltype(t.first)>(t.first);
    }
};

}  // namespace mini::func

#endif
