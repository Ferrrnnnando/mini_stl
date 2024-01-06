#include "mini_stl/test/mini_unittest.h"

#include "mini_stl/functional/mini_functional.h"
#include "mini_stl/utility/mini_utility_hashtable.h"

#include <functional>

TEST(mini_utility_test, hashtable_test_basics)
{
    using value_type = int;
    using key_type = value_type;
    using hash_type = std::hash<key_type>;
    using extract_type = mini::func::identity;
    using equal_type = std::equal_to<key_type>;
    using allocator_type = mini::mem::alloc;
    using hashtable = mini::util::hashtable<key_type, value_type, hash_type, extract_type,
        equal_type, allocator_type>;

    {
        const size_t seek_size = 50;
        size_t actual_size = mini::__mini_stl_next_prime(seek_size);
        hashtable ht(seek_size, hash_type(), equal_type());

        EXPECT_EQ(actual_size, 53);
        EXPECT_EQ(ht.bucket_count(), actual_size);
        EXPECT_EQ(ht.max_bucket_count(), 805306457);
        EXPECT_EQ(ht.size(), 0);

        // insert elements
        ht.insert_unique(59);
        ht.insert_unique(63);
        ht.insert_unique(108);
        ht.insert_unique(2);
        ht.insert_unique(53);
        ht.insert_unique(55);
        // insert duplicated
        ht.insert_unique(55);
        EXPECT_EQ(ht.size(), 6);

        // iterations
        EXPECT_EQ(dump(ht), "53 55 2 108 59 63");

        // number of elements in each bucket
        EXPECT_EQ(ht.elements_in_bucket(0), 1);
        EXPECT_EQ(ht.elements_in_bucket(2), 3);
        EXPECT_EQ(ht.elements_in_bucket(6), 1);
        EXPECT_EQ(ht.elements_in_bucket(10), 1);

        // add more elements to see if rebuild of table will happen
        for (size_t i = 0; i < 48; i++) {
            ht.insert_equal(i);
        }
        EXPECT_EQ(ht.size(), 54);
        EXPECT_EQ(ht.bucket_count(), 97);

        // find operations
        EXPECT_EQ(*(ht.find(108)), 108);  // find element with key=108
        EXPECT_EQ(ht.count(2), 2);        // number of elements with key=2
    }
}
