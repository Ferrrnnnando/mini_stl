#include "mini_stl/test/mini_unittest.h"

#include "mini_stl/container/mini_container_map.h"

#include <string>

TEST(mini_container_test, map_test_basics)
{
    using key_type = const std::string;
    using value_type = int;
    using element_type = mini::util::pair<key_type, value_type>;
    using Compare = mini::func::less<key_type>;
    using map = mini::ctnr::map<key_type, value_type, Compare>;

    {
        map m;
        EXPECT_EQ(m.size(), 0);

        // insert a pair
        key_type k1 = "1";
        value_type v1 = 1;
        element_type e1(k1, v1);
        auto ret11 = m.insert(e1);
        EXPECT_EQ((*(ret11.first)).first, k1);
        EXPECT_EQ((*(ret11.first)).second, v1);

        // use operator[] to add a pair
        key_type k2 = "2";
        value_type v2 = 2;
        m[k2] = v2;
        auto ret21 = m.find(k2);
        EXPECT_EQ((*ret21).first, k2);
        EXPECT_EQ((*ret21).second, v2);

        // modify a pair's value
        v2 = 20;
        m[k2] = v2;
        auto ret22 = m.find(k2);
        EXPECT_EQ((*ret22).first, k2);
        // (*ret22).first = "new key";  // cannot assign value to a const key
        EXPECT_EQ((*ret22).second, v2);
        // (*ret22).second = 100;   // valid
    }
}
