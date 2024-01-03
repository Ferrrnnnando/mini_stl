#include "mini_stl/test/mini_unittest.h"

#include "mini_stl/utility/mini_utility_rbtree.h"

TEST(mini_utility_rb_tree_test, basics)
{
    using value_type = int;
    using key_type = value_type;
    using KeyOfValue = mini::func::identity;
    using Compare = mini::func::less<key_type>;
    using rb_tree = mini::util::rb_tree<key_type, value_type, KeyOfValue, Compare>;

    {
        rb_tree rbt;
        EXPECT_EQ(rbt.size(), 0);

        auto pair1 = rbt.insert_unique(10);
        EXPECT_EQ(*(pair1.first), 10);
        EXPECT_TRUE(pair1.second);

        auto pair2 = rbt.insert_unique(10);
        EXPECT_FALSE(pair2.second);

        rbt.insert_unique(7);
        rbt.insert_unique(8);

        rbt.insert_unique(15);
        rbt.insert_unique(5);
        rbt.insert_unique(6);

        rbt.insert_unique(11);
        rbt.insert_unique(13);
        rbt.insert_unique(12);

        EXPECT_EQ(rbt.size(), 9);
        EXPECT_EQ(dump(rbt), "5 6 7 8 10 11 12 13 15");

        // find
        EXPECT_EQ(*rbt.find(8), 8);
        EXPECT_EQ(rbt.find(9), rbt.end());

        // modify
        auto ret = rbt.find(8);
        *ret = 999;
        // note: tree's properties may be affected: no longer sorted
        EXPECT_EQ(dump(rbt), "5 6 7 999 10 11 12 13 15");
    }
}
