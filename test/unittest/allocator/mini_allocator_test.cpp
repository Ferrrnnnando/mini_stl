#include "mini_stl/allocator/mini_alloc_base.h"
#include "mini_stl/test/mini_unittest.h"

#include <iostream>
#include <vector>

using namespace std;

TEST(mini_allocator, primitive_type)
{
    int arr[] = {1, 2, 3};
    size_t len = sizeof(arr) / sizeof(int);
    vector<int, mini::memory::allocator<int>> vec(arr, arr + len);
    for (auto e : vec) {
        cout << e << endl;
    }
}

TEST(mini_allocator, nonprimitive_type)
{
    struct A {
        int a_;
        char b_;
        A(int a, char b)
            : a_(a)
            , b_(b)
        {
            cout << "A's ctor" << endl;
        }
    };
    vector<A, mini::memory::allocator<A>> vec2;
    vec2.emplace_back(1, 'a');
    for (auto& e : vec2) {
        cout << e.a_ << " " << e.b_ << endl;
    }
}
