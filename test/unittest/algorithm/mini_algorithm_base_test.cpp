#include "mini_stl/test/mini_unittest.h"

#include "mini_stl/algorithm/mini_algorithm_base.h"
#include "mini_stl/container/mini_container_vector.h"

#include <algorithm>

TEST(mini_algo_test, copy)
{
    mini::ctnr::vector<int> vec1(3, 1);
    mini::ctnr::vector<int> vec2(3);

    mini::algo::copy(vec1.begin(), vec1.end(), vec2.begin());
    EXPECT_TRUE(std::equal(vec1.begin(), vec1.end(), vec2.begin()));
}
