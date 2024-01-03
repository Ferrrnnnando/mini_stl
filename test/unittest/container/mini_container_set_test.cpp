#include "mini_stl/test/mini_unittest.h"

#include "mini_stl/container/mini_container_set.h"

TEST(mini_container_set_test, basics)
{
    using value_type = int;
    using key_type = value_type;
    using Compare = mini::func::less<key_type>;
    using set = mini::ctnr::set<key_type, Compare>;

    {
        set s;
        EXPECT_EQ(s.size(), 0);

        // insert
        auto ret1 = s.insert(3);
        EXPECT_EQ(*(ret1.first), 3);
        EXPECT_TRUE(ret1.second);
        EXPECT_EQ(s.size(), 1);

        auto ret2 = s.insert(3);
        EXPECT_FALSE(ret2.second);
        EXPECT_EQ(s.size(), 1);

        auto ret3 = s.insert(5);
        EXPECT_EQ(*(ret3.first), 5);
        EXPECT_TRUE(ret3.second);
        EXPECT_EQ(s.size(), 2);

        // find
        auto ret4 = s.find(3);
        EXPECT_EQ(*ret4, 3);

        auto ret5 = s.find(7);
        EXPECT_EQ(ret5, s.end());

        // change value
        // *ret4 = 10;  // error: cannot assign to return value because it's a const value
    }
}
