#include "mini_stl/test/mini_unittest.h"

#include "mini_stl/algorithm/mini_algorithm.h"
#include "mini_stl/container/mini_container_list.h"

TEST(mini_container_list_test, basics)
{
    using value_type = int;
    using allocator = mini::mem::alloc;
    using list = mini::ctnr::list<value_type, allocator>;

    {
        // empty initialize
        list l;
        EXPECT_EQ(l.size(), 0);
        EXPECT_EQ(l.dump(), "");
    }

    {
        list l;

        // append some elements
        size_t NUM_NODE = 10;
        for (size_t i = 0; i < NUM_NODE; ++i) {
            l.push_back(i);
        }
        EXPECT_EQ(l.size(), NUM_NODE);
        EXPECT_EQ(l.dump(), "0 1 2 3 4 5 6 7 8 9");

        // pop some elements
        l.pop_front();
        l.pop_back();
        NUM_NODE -= 2;
        EXPECT_EQ(l.size(), NUM_NODE);
        EXPECT_EQ(l.dump(), "1 2 3 4 5 6 7 8");

        // erase some elements
        auto it = mini::algo::find(l.begin(), l.end(), 3);
        l.erase(it);  // node with value 3 is erased
        NUM_NODE -= 1;
        EXPECT_EQ(l.size(), NUM_NODE);
        EXPECT_EQ(l.dump(), "1 2 4 5 6 7 8");
    }
}

TEST(mini_container_list_test, member_func_splice)
{
    using value_type = int;
    using allocator = mini::mem::alloc;
    using list = mini::ctnr::list<value_type, allocator>;

    {
        // test 1: transfer another list to current list
        size_t NUM_NODE = 0;

        // l initial status
        list l;
        l.push_back(1);
        l.push_back(3);
        l.push_back(3);
        l.push_back(4);
        l.push_back(6);
        auto it11 = l.begin();
        auto it12 = it11;
        mini::iter::advance(it12, 0);
        EXPECT_EQ(*it11, 1);
        EXPECT_EQ(*it12, 1);
        NUM_NODE += 5;
        EXPECT_EQ(l.size(), NUM_NODE);
        EXPECT_EQ(l.dump(), "1 3 3 4 6");

        // l2 initial status
        list l2;
        l2.push_back(0);
        l2.push_back(3);
        l2.push_back(5);
        l2.push_back(9);
        auto it21 = l2.begin();
        auto it22 = it21;
        mini::iter::advance(it22, 2);
        EXPECT_EQ(*it21, 0);
        EXPECT_EQ(*it22, 5);
        EXPECT_EQ(l2.size(), 4);
        EXPECT_EQ(l2.dump(), "0 3 5 9");

        // Operations taken
        l.splice(l.begin(), l2);  // insert l2 to front of l
        NUM_NODE += 4;

        // Expected results
        EXPECT_EQ(l.size(), NUM_NODE);
        EXPECT_EQ(l.dump(), "0 3 5 9 1 3 3 4 6");
        EXPECT_EQ(l2.size(), 0);  // l2 should become empty after transfering all elements
        EXPECT_EQ(l2.dump(), "");
        EXPECT_EQ(*it11, 1);  // No iterators and references become invalidated
        EXPECT_EQ(*it12, 1);
        EXPECT_EQ(*it21, 0);
        EXPECT_EQ(*it22, 5);
    }

    {
        // test 2: transfer an element to another position
        list l;
        size_t NUM_NODE = 0;

        NUM_NODE += 10;
        for (size_t i = 0; i < NUM_NODE; ++i) {
            l.push_back(i);
        }
        EXPECT_EQ(l.size(), NUM_NODE);
        EXPECT_EQ(l.dump(), "0 1 2 3 4 5 6 7 8 9");

        // case 1: same list
        auto it1 = l.begin();
        EXPECT_EQ(*it1, 0);
        l.splice(l.end(), l, it1);
        EXPECT_EQ(l.size(), NUM_NODE);
        EXPECT_EQ(l.dump(), "1 2 3 4 5 6 7 8 9 0");
        EXPECT_EQ(*it1, 0);  // iterator still valid

        // ccase 2: different list
        list l2;
        l2.push_back(2);
        l2.push_back(5);
        EXPECT_EQ(l2.size(), 2);
        EXPECT_EQ(l2.dump(), "2 5");
        l.splice(++l.begin(), l2, --l2.end());  // move '5' in l2 to second position in l
        NUM_NODE += 1;
        EXPECT_EQ(l.size(), NUM_NODE);
        EXPECT_EQ(l.dump(), "1 5 2 3 4 5 6 7 8 9 0");
        EXPECT_EQ(l2.size(), 1);
        EXPECT_EQ(l2.dump(), "2");
    }

    {
        // test 3: move a range to another position in the same list
        list l;
        size_t NUM_NODE = 0;

        NUM_NODE += 10;
        for (size_t i = 0; i < NUM_NODE; ++i) {
            l.push_back(i);
        }
        EXPECT_EQ(l.size(), NUM_NODE);
        EXPECT_EQ(l.dump(), "0 1 2 3 4 5 6 7 8 9");

        auto first = mini::algo::find(l.begin(), l.end(), 3);
        auto last = mini::algo::find(l.begin(), l.end(), 6);
        l.splice(l.begin(), l, first, last);
        EXPECT_EQ(l.size(), NUM_NODE);
        EXPECT_EQ(l.dump(), "3 4 5 0 1 2 6 7 8 9");
    }
}

TEST(mini_container_list_test, advanced)
{
    using value_type = int;
    using allocator = mini::mem::alloc;
    using list = mini::ctnr::list<value_type, allocator>;

    {
        list l;
        size_t NUM_NODE = 0;

        l.push_back(1);
        l.push_back(3);
        l.push_back(3);
        l.push_back(4);
        l.push_back(6);
        NUM_NODE += 5;
        EXPECT_EQ(l.size(), NUM_NODE);
        EXPECT_EQ(l.dump(), "1 3 3 4 6");

        // merge another list into current list
        list l2;
        l2.push_back(0);
        l2.push_back(3);
        l2.push_back(5);
        l2.push_back(9);
        EXPECT_EQ(l2.size(), 4);
        EXPECT_EQ(l2.dump(), "0 3 5 9");

        l.merge(l2);
        NUM_NODE += 4;
        EXPECT_EQ(l.size(), NUM_NODE);
        EXPECT_EQ(l.dump(), "0 1 3 3 3 4 5 6 9");
    }
}
