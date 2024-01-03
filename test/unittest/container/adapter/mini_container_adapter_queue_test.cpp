#include "mini_stl/test/mini_unittest.h"

#include "mini_stl/algorithm/mini_algorithm.h"
#include "mini_stl/container/adapter/mini_container_adapter_queue.h"
#include "mini_stl/container/mini_container_list.h"

TEST(mini_container_test, adapter_queue_test_basics)
{
    using value_type = int;
    using sequence = mini::ctnr::list<value_type>;
    using queue = mini::ctnr::queue<value_type, sequence>;

    {
        queue q;
        EXPECT_TRUE(q.empty());
        EXPECT_EQ(q.size(), 0);

        q.push(1);
        q.push(2);
        EXPECT_EQ(q.size(), 2);
        EXPECT_EQ(q.front(), 1);
        EXPECT_EQ(q.back(), 2);

        q.pop();
        EXPECT_EQ(q.size(), 1);
        EXPECT_EQ(q.front(), 2);
        EXPECT_EQ(q.back(), 2);
    }
}
