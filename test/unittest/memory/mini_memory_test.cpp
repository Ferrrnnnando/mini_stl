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
    using value_type = int;
    using allocator_type = mini::memory::alloc;

    // std::vector<value_type, mini::memory::simple_alloc<value_type, allocator_type>> vec;
}
