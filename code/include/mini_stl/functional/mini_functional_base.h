#ifndef MINI_FUNCTIONAL_BASE_H
#define MINI_FUNCTIONAL_BASE_H

namespace mini::func {

template<typename Arg, typename Result>
struct unary_function {
    using argument_type = Arg;
    using result_type = Result;
};

template<typename Arg1, typename Arg2, typename Result>
struct binary_function {
    using first_argument_type = Arg1;
    using second_argument_type = Arg2;
    using result_type = Result;
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

template<typename Arg1, typename Arg2>
struct project1st : public binary_function<Arg1, Arg2, Arg1> {
    Arg1 operator()(const Arg1& x, const Arg2& /* y */) const { return x; }
};

template<typename Arg1, typename Arg2>
struct project2nd : public binary_function<Arg1, Arg2, Arg2> {
    Arg2 operator()(const Arg1& /* x */, const Arg2& y) const { return y; }
};

}  // namespace mini::func

#endif
