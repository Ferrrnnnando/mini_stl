#include "mini_stl/test/mini_unittest.h"

#include "mini_stl/container/mini_container_vector.h"

TEST(mini_container_test, vector)
{
    using value_type = int;
    using allocator = mini::memory::alloc;
    using vector = mini::container::vector<value_type, allocator>;

    {
        // Add elements one by one
        vector vec;
        EXPECT_EQ(vec.size(), 0);

        vec.push_back(1);
        EXPECT_EQ(vec.size(), 1);
        EXPECT_EQ(vec.capacity(), 1);

        vec.push_back(2);
        EXPECT_EQ(vec.size(), 2);
        EXPECT_EQ(vec.capacity(), 2);

        vec.push_back(3);
        EXPECT_EQ(vec.size(), 3);
        EXPECT_EQ(vec.capacity(), 4);

        vec.push_back(4);
        EXPECT_EQ(vec.size(), 4);
        EXPECT_EQ(vec.capacity(), 4);

        vec.push_back(5);
        EXPECT_EQ(vec.size(), 5);
        EXPECT_EQ(vec.capacity(), 8);

        // insert elements
        vec.insert(vec.begin(), 5, 10);
        for (int i = 0; i < 5; i++) {
            EXPECT_EQ(vec[i], 10);
        }
        EXPECT_EQ(vec.size(), 10);
        EXPECT_EQ(vec.capacity(), 10);

        // clear all elements
        vec.clear();
        EXPECT_EQ(vec.size(), 0);
        EXPECT_EQ(vec.capacity(), 10);
        EXPECT_TRUE(vec.empty());

        // insert elements more than reserved space
        vec.insert(vec.begin(), 12, 20);
        for (int i = 0; i < 5; i++) {
            EXPECT_EQ(vec[i], 20);
        }
        EXPECT_EQ(vec.size(), 12);
        EXPECT_EQ(vec.capacity(), 12);

        for (int i = 0; i < vec.size(); i++) {
            vec[i] = i;
        }
        // erase single element
        EXPECT_EQ(vec[10], 10);
        vec.erase(vec.begin() + 10);
        EXPECT_EQ(vec[9], 9);
        EXPECT_EQ(vec[10], 11);

        // erase multiple elements in range: [first, last)
        vec.erase(vec.begin() + 1, vec.begin() + 3);
        EXPECT_EQ(vec[0], 0);
        EXPECT_EQ(vec[1], 3);
        EXPECT_EQ(vec[2], 4);
    }
}
