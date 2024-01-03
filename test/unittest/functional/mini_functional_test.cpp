#include "mini_stl/test/mini_unittest.h"

#include "mini_stl/functional/mini_functional.h"

TEST(mini_functional_test, comparator)
{
    // warning: implicit conversion from 'double' to 'int'
    // static_assert(mini::func::less<int>{}(5, 5.6) == false);

    static_assert(mini::func::less<double>{}(5, 5.6) == true);
    static_assert(mini::func::less{}(5, 5.6) == true);
}
