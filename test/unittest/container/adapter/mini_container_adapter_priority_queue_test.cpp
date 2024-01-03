#include "mini_stl/test/mini_unittest.h"

#include "mini_stl/container/adapter/mini_container_adapter_priority_queue.h"

TEST(mini_container_test, adapter_priority_queue_test_std_vector)
{
    using value_type = int;
    using priority_queue = mini::ctnr::priority_queue<value_type>;

    {
        priority_queue q;
        q.push(0);
        q.push(1);
        q.push(3);
        q.push(4);
        q.push(8);
        q.push(9);
        q.push(3);
        q.push(5);
        EXPECT_EQ(q.top(), 9);
    }
}

TEST(mini_container_test, adapter_priority_queue_test_array)
{
    using value_type = int;
    using priority_queue = mini::ctnr::priority_queue<value_type>;

    {
        // Array as container
        value_type arr[] = {0, 1, 2, 3, 4, 8, 9, 3, 5};
        const size_t size = sizeof(arr) / sizeof(value_type);
        priority_queue q(arr, arr + size);
        EXPECT_EQ(q.top(), 9);

        // pop and push
        q.pop();
        EXPECT_EQ(q.top(), 8);
        q.pop();
        EXPECT_EQ(q.top(), 5);

        q.push(-3);
        EXPECT_EQ(q.top(), 5);
        q.push(10);
        EXPECT_EQ(q.top(), 10);
    }
}
