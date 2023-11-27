#include "mini_stl/test/mini_unittest.h"

#include "mini_stl/container/mini_container_vector.h"

#include <algorithm>

TEST(mini_container_vector_test, primitive_types)
{
    using value_type = int;
    using allocator = mini::mem::alloc;
    using vector = mini::ctnr::vector<value_type, allocator>;

    {
        vector vec;
    }

    {
        // Different vector constructors
        vector vec(2);
        EXPECT_EQ(vec.size(), 2);
        EXPECT_EQ(vec.capacity(), 2);

        vector vec2(2, 10);
        EXPECT_EQ(vec.size(), 2);
        EXPECT_EQ(vec.capacity(), 2);
        EXPECT_EQ(vec2[0], 10);
        EXPECT_EQ(vec2[1], 10);

        // out-of-range access
        try {
            vec2.at(2);
        } catch (const std::exception& e) {
            EXPECT_STREQ(e.what(), "mini::ctnr::vector: out_of_range failure: n >= size()");
        }
    }

    {
        // Add elements one by one
        vector vec;
        EXPECT_EQ(vec.size(), 0);

        vec.push_back(1);
        EXPECT_EQ(vec[0], 1);
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

    {
        vector vec(2, 3);
        int arr[] = {3, 3};
        auto data_ptr = vec.data();
        EXPECT_TRUE(std::equal(std::begin(arr), std::end(arr), data_ptr));
    }

    {
        // reserve test #1
        vector vec;
        EXPECT_EQ(vec.size(), 0);
        EXPECT_EQ(vec.capacity(), 0);
        vec.reserve(0);   // does nothing
        vec.reserve(10);  // capacity increased to 10
        EXPECT_EQ(vec.size(), 0);
        EXPECT_EQ(vec.capacity(), 10);

        // reserve test #2
        vector vec2(2, 2);
        EXPECT_EQ(vec2.size(), 2);
        EXPECT_EQ(vec2.capacity(), 2);
        vec2.reserve(1);  // does nothing
        vec2.reserve(3);  // capacity increased to 3
        EXPECT_EQ(vec2.size(), 2);
        EXPECT_EQ(vec2.capacity(), 3);
        EXPECT_EQ(vec2[0], 2);
        EXPECT_EQ(vec2[1], 2);
    }
}

TEST(mini_container_vector_test, nonprimive_types)
{
    struct A {
        ~A() { std::cout << "A's dtor" << std::endl; }
    };

    using value_type = A;
    using allocator = mini::mem::alloc;
    using vector = mini::ctnr::vector<value_type, allocator>;

    {
        vector vec;
    }
}
