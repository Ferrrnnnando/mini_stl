#include "mini_stl/test/mini_unittest.h"

#include "mini_stl/algorithm/mini_algorithm.h"

TEST(mini_algo_test, heap_test_std_vector)
{
    // Use std::vector as underlying container
    std::vector<int> v{0, 1, 2, 3, 4, 8, 9, 3, 5};
    mini::algo::make_heap(v.begin(), v.end());
    EXPECT_EQ(dump(v), "9 5 8 3 4 0 2 3 1");

    v.push_back(7);
    mini::algo::push_heap(v.begin(), v.end());
    EXPECT_EQ(dump(v), "9 7 8 3 5 0 2 3 1 4");

    mini::algo::pop_heap(v.begin(), v.end());
    EXPECT_EQ(v.back(), 9);
    v.pop_back();
    EXPECT_EQ(dump(v), "8 7 4 3 5 0 2 3 1");

    mini::algo::sort_heap(v.begin(), v.end());
    EXPECT_EQ(dump(v), "0 1 2 3 3 4 5 7 8");
}

TEST(mini_algo_test, heap_test_array)
{
    // Use raw array as underlying container
    using value_type = int;
    value_type v[] = {0, 1, 2, 3, 4, 8, 9, 3, 5};
    const size_t size = sizeof(v) / sizeof(value_type);

    mini::algo::make_heap(v, v + size);
    EXPECT_EQ(dump(v), "9 5 8 3 4 0 2 3 1");

    mini::algo::sort_heap(v, v + size);
    EXPECT_EQ(dump(v), "0 1 2 3 3 4 5 8 9");
}
