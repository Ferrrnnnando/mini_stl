#include "mini_stl/test/mini_unittest.h"

#include "mini_stl/container/mini_container_unordered_set.h"
#include "mini_stl/functional/mini_functional.h"

TEST(mini_container_test, unordered_set_test_basics)
{
    using value_type = const char*;
    using hash_type = std::hash<value_type>;
    using equal_type = mini::func::equal_to<value_type>;
    using allocator_type = mini::mem::alloc;
    using unordered_set =
        mini::ctnr::unordered_set<value_type, hash_type, equal_type, allocator_type>;

    {
        unordered_set us;
        EXPECT_EQ(us.size(), 0);
        EXPECT_EQ(us.bucket_count(), 193);

        // insertions
        us.insert("kiwi");
        us.insert("plum");
        us.insert("apple");
        us.insert("mango");
        us.insert("apricot");
        us.insert("banana");
        EXPECT_EQ(us.size(), 6);

        auto it = us.insert("banana");
        EXPECT_FALSE(it.second);
        EXPECT_EQ(us.size(), 6);

        // range insertions
        const char* fruits[2] = {"orange", "peach"};
        us.insert(fruits, fruits + 2);
        EXPECT_EQ(us.size(), 8);

        unordered_set us2;
        us2.insert("pineapple");
        us2.insert("apple");  // duplicated with a key in us
        EXPECT_EQ(us2.size(), 2);
        us.insert(us2.begin(), us2.end());
        EXPECT_EQ(us.size(), 8 + 1);
    }
}
