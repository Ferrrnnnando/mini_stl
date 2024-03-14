#include "mini_stl/test/mini_unittest.h"

#include "mini_stl/functional/mini_functional.h"
#include "mini_stl/utility/mini_utility.h"

TEST(mini_functional_test, base)
{
    static_assert(mini::func::select1st()(mini::util::pair<int, double>(1, 2.0)) == 1);
}

TEST(mini_functional_test, comparator)
{
    // warning: implicit conversion from 'double' to 'int'
    // static_assert(mini::func::less<int>{}(5, 5.6) == false);

    static_assert(mini::func::less<double>{}(5, 5.6) == true);
    static_assert(mini::func::less{}(5, 5.6) == true);
}

TEST(mini_functional_test, adapters)
{
    // binder1st function object: bind an argument to first parameter of function,
    // and accept another argument as second parameter of function.
    auto binder = mini::func::bind1st(mini::func::equal_to<int>(), 1);
    EXPECT_TRUE(binder(1));
    EXPECT_FALSE(binder(2));
}
