#include "mini_stl/test/mini_unittest.h"

#include "mini_stl/algorithm/mini_algorithm.h"
#include "mini_stl/container/adapter/mini_container_adapter_stack.h"
#include "mini_stl/container/mini_container_list.h"

TEST(mini_container_test, adapter_stack_test_basics)
{
    using value_type = int;
    using sequence = mini::ctnr::list<value_type>;
    using stack = mini::ctnr::stack<value_type, sequence>;

    {
        stack st;
        EXPECT_TRUE(st.empty());
        EXPECT_EQ(st.size(), 0);

        st.push(1);
        st.push(2);
        EXPECT_EQ(st.size(), 2);
        EXPECT_EQ(st.top(), 2);

        st.pop();
        EXPECT_EQ(st.size(), 1);
        EXPECT_EQ(st.top(), 1);

        st.pop();
        EXPECT_TRUE(st.empty());
    }
}
