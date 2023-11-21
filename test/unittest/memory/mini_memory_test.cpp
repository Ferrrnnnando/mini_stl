#include "mini_stl/memory/mini_memory_alloc.h"
#include "mini_stl/memory/mini_memory_defalloc.h"
#include "mini_stl/test/mini_unittest.h"

#include <iostream>
#include <vector>

TEST(mini_memory_defalloc, primitive_type)
{
    int arr[] = {1, 2, 3};
    size_t len = sizeof(arr) / sizeof(int);
    std::vector<int, mini::memory::allocator<int>> vec(arr, arr + len);
    for (auto e : vec) {
        std::cout << e << std::endl;
    }
}

TEST(mini_memory_defalloc, nonprimitive_type)
{
    struct A {
        int a_;
        char b_;
        A(int a, char b)
            : a_(a)
            , b_(b)
        {
            std::cout << "A's ctor" << std::endl;
        }
    };
    std::vector<A, mini::memory::allocator<A>> vec2;
    vec2.emplace_back(1, 'a');
    for (auto& e : vec2) {
        std::cout << e.a_ << " " << e.b_ << std::endl;
    }
}

TEST(mini_memory_alloc, primitive_type)
{
    using allocator_type = mini::memory::alloc;

    {
        using value_type = uint32_t;
        using allocator = mini::memory::simple_alloc<value_type, allocator_type>;

        // A chunk of consecutive memory is allocated from heap, so memory addresses returned
        // in a few subsequent allocation calls should be consecutive.
        auto ptr1 = allocator::allocate();
        auto ptr2 = allocator::allocate();
        auto ptr3 = allocator::allocate();

        // Pointer difference is 2, means 8 bytes.
        // 'int' type only requires 4 bytes, but internally allocator align this size to 8
        EXPECT_EQ((ptr2 - ptr1), 2);
        EXPECT_EQ((ptr3 - ptr2), 2);
    }

    {
        using value_type = __uint128_t;
        using allocator = mini::memory::simple_alloc<value_type, allocator_type>;

        auto ptr1 = allocator::allocate();
        auto ptr2 = allocator::allocate();
        auto ptr3 = allocator::allocate();

        // Pointer difference is 1, means 16 bytes
        EXPECT_EQ((ptr2 - ptr1), 1);
        EXPECT_EQ((ptr3 - ptr2), 1);
    }
}
