#include "mini_stl/test/mini_unittest.h"

#include "mini_stl/container/mini_container_vector.h"

#include <algorithm>

TEST(mini_container_test, vector_test_primitive_types)
{
    using value_type = int;
    using allocator = mini::mem::alloc;
    using vector = mini::ctnr::vector<value_type, allocator>;

    {
        // Different vector constructors
        vector vec0;
        EXPECT_EQ(vec0.size(), 0);
        EXPECT_EQ(vec0.capacity(), 0);

        // Construct with a size
        vector vec1(2);
        EXPECT_EQ(vec1.size(), 2);
        EXPECT_EQ(vec1.capacity(), 2);

        // Construct with a size and initial values
        vector vec2(2, 10);
        EXPECT_EQ(vec2.size(), 2);
        EXPECT_EQ(vec2.capacity(), 2);
        EXPECT_EQ(dump(vec2), "10 10");

        // Construct using existing containers
        value_type arr[] = {1, 2, 3};
        const size_t size = sizeof(arr) / sizeof(value_type);
        vector vec3(arr, arr + 3);
        EXPECT_EQ(vec3.size(), size);
        EXPECT_EQ(dump(vec3), "1 2 3");

        vector vec4(vec2.begin(), vec2.end());
        EXPECT_EQ(vec4.size(), 2);
        EXPECT_EQ(dump(vec4), "10 10");

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
        EXPECT_EQ(dump(vec), "10 10 10 10 10 1 2 3 4 5");
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

TEST(mini_container_test, vector_test_nonprimive_types)
{
    struct A {
        int foo;
        char bar;

        A(int foo, char bar)
        {
            this->foo = foo;
            this->bar = bar;
        }
        // ~A() { std::cout << "A's dtor" << std::endl; }

        bool operator==(const A& rhs) const { return foo == rhs.foo && bar == rhs.bar; }
    };

    using value_type = A;
    using allocator = mini::mem::alloc;
    using vector = mini::ctnr::vector<value_type, allocator>;

    {
        vector vec;
    }

    {
        vector vec;
        vec.push_back(A(1, 'a'));
        vec.push_back(A(2, 'b'));

        vector vec2;
        vec2.push_back(A(1, 'a'));
        vec2.push_back(A(2, 'b'));

        EXPECT_TRUE(std::equal(vec.begin(), vec.end(), vec2.begin(), vec2.end()));
    }
}
