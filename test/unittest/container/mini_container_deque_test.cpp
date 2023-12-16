#include "mini_stl/test/mini_unittest.h"

#include "mini_stl/algorithm/mini_algorithm.h"
#include "mini_stl/container/mini_container_deque.h"

TEST(mini_container_deque_test, basics)
{
    using value_type = int;
    using allocator = mini::mem::alloc;
    const size_t BUFFER_SIZE = 32;
    using deque = mini::ctnr::deque<value_type, allocator, BUFFER_SIZE>;

    {
        // creation
        deque d(0, 0);
        EXPECT_EQ(d.size(), 0);
        EXPECT_EQ(d.dump(), "");

        deque d2(5, 6);
        EXPECT_EQ(d2.size(), 5);
        EXPECT_EQ(d2.dump(), "6 6 6 6 6");

        // elements access and travesal
        for (size_t i = 0; i < d2.size(); ++i) {
            d2[i] = i;
        }
        EXPECT_EQ(d2.dump(), "0 1 2 3 4");

        for (auto& e : d2) {
            e = 0;
        }
        EXPECT_EQ(d2.dump(), "0 0 0 0 0");
    }
}

TEST(mini_container_deque_test, advanced)
{
    using value_type = int;
    using allocator = mini::mem::alloc;
    const size_t BUFFER_SIZE = 3;  // fewer elements per buffer
    using deque = mini::ctnr::deque<value_type, allocator, BUFFER_SIZE>;

    {
        // push_back, push_front
        deque d(0, 0);
        d.push_back(10);
        d.push_front(20);
        EXPECT_EQ(d.size(), 2);
        EXPECT_EQ(d.dump(), "20 10");

        // find
        value_type find_value = 20;
        auto iter = mini::algo::find(d.begin(), d.end(), find_value);
        EXPECT_EQ(*iter, find_value);

        // pop_back, pop_front
        EXPECT_EQ(d.front(), 20);
        EXPECT_EQ(d.back(), 10);
        d.pop_front();
        EXPECT_EQ(d.dump(), "10");
        d.pop_back();
        EXPECT_EQ(d.dump(), "");
    }

    {
        deque d(3, 1);

        deque d2(8, 6);
        EXPECT_EQ(d2.size(), 8);
        EXPECT_EQ(d2.dump(), "6 6 6 6 6 6 6 6");
        for (size_t i = 0; i < d2.size(); ++i) {
            d2[i] = i;
        }

        // clear
        EXPECT_EQ(d.dump(), "1 1 1");
        d.clear();
        EXPECT_EQ(d.size(), 0);
        EXPECT_EQ(d.dump(), "");

        // erase
        EXPECT_EQ(d2.dump(), "0 1 2 3 4 5 6 7");

        deque::iterator res;
        res = d2.erase(d2.end() - 2);  // erase front half
        EXPECT_EQ(*res, 7);
        EXPECT_EQ(d2.dump(), "0 1 2 3 4 5 7");

        res = d2.erase(d2.begin() + 2);  // erase back half
        EXPECT_EQ(*res, 3);
        EXPECT_EQ(d2.dump(), "0 1 3 4 5 7");

        auto first = mini::algo::find(d2.begin(), d2.end(), 3);
        auto last = mini::algo::find(d2.begin(), d2.end(), 5) + 1;
        res = d2.erase(first, last);  // erase interval
        EXPECT_EQ(*res, 7);
        EXPECT_EQ(d2.dump(), "0 1 7");

        // insert
        EXPECT_EQ(d2.dump(), "0 1 7");

        res = d2.insert(d2.begin() + 1, 100);  // insert front half
        EXPECT_EQ(*res, 100);
        EXPECT_EQ(d2.dump(), "0 100 1 7");

        res = d2.insert(d2.end() - 1, 6);  // insert back half
        EXPECT_EQ(*res, 6);
        EXPECT_EQ(d2.dump(), "0 100 1 6 7");

        res = d2.insert(d2.begin(), -3);  // insert at front
        EXPECT_EQ(*res, -3);
        EXPECT_EQ(d2.dump(), "-3 0 100 1 6 7");

        res = d2.insert(d2.end(), -1);  // insert at back
        EXPECT_EQ(*res, -1);
        EXPECT_EQ(d2.dump(), "-3 0 100 1 6 7 -1");
    }
}
