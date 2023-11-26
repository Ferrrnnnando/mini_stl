#include "mini_stl/test/mini_unittest.h"

#include "mini_stl/memory/mini_memory.h"
#include "mini_stl/memory/mini_memory_defalloc.h"

#include <algorithm>
#include <vector>

TEST(mini_memory_defalloc_test, vec_primitive_type_defalloc)
{
    int arr[] = {1, 2, 3};
    size_t len = sizeof(arr) / sizeof(int);
    std::vector<int, mini::memory::allocator<int>> vec(arr, arr + len);

    std::vector ans{1, 2, 3};
    EXPECT_TRUE(std::equal(vec.begin(), vec.end(), ans.begin()));
}

TEST(mini_memory_defalloc_test, vec_nonprimitive_type_defalloc)
{
    struct A {
        A(int a, char b)
            : a_(a)
            , b_(b)
        {}
        bool operator==(const A& rhs) { return a_ == rhs.a_ && b_ == rhs.b_; }

        int a_;
        char b_;
    };
    std::vector<A, mini::memory::allocator<A>> vec2;
    vec2.emplace_back(1, 'a');
    EXPECT_TRUE(vec2.back() == A(1, 'a'));
}

TEST(mini_memory_alloc_test, primitive_type)
{
    typedef mini::memory::__default_alloc_template<false, 1> alloc;
    using allocator_type = alloc;

    {
        using value_type = uint32_t;
        using allocator = mini::memory::simple_alloc<value_type, allocator_type>;

        // Avoid deallocating nullptr
        allocator::deallocate(NULL);
    }

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
