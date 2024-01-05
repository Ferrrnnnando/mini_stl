#include "mini_stl/test/mini_unittest.h"

#include "mini_stl/utility/mini_utility_rbtree.h"

#include <string>
#include <vector>

namespace {

template<typename Key, typename Value, typename KeyOfValue,
    typename Compare = mini::func::less<Key>, typename Allocator = mini::mem::alloc>
class rb_tree_test_class : public mini::util::rb_tree<Key, Value, KeyOfValue, Compare, Allocator> {
public:
    using base_class = mini::util::rb_tree<Key, Value, KeyOfValue, Compare, Allocator>;
    using link_type = typename base_class::link_type;

public:
    std::vector<std::string> get_root_to_leaf_paths()
    {
        auto node = base_class::root();
        return helper(node);
    }

private:
    std::vector<std::string> helper(link_type node)
    {
        if (!node) {
            return {};
        }

        auto left = base_class::left(node);
        auto right = base_class::right(node);
        auto value = base_class::value(node);

        if (!left && !right) {  // leaf node
            return {std::to_string(value)};
        }

        auto paths = helper(left);
        auto right_paths = helper(right);
        paths.insert(paths.end(), right_paths.begin(), right_paths.end());
        for (auto& path : paths) {
            path = std::to_string(value) + " " + path;
        }

        return paths;
    }
};

}  // namespace

TEST(mini_utility_test, rb_tree_test_basics)
{
    using value_type = int;
    using key_type = value_type;
    using KeyOfValue = mini::func::identity;
    using Compare = mini::func::less<key_type>;
    using rb_tree = rb_tree_test_class<key_type, value_type, KeyOfValue, Compare>;

    {
        rb_tree rbt;
        EXPECT_EQ(rbt.size(), 0);

        auto pair1 = rbt.insert_unique(10);
        EXPECT_EQ(*(pair1.first), 10);
        EXPECT_TRUE(pair1.second);

        auto pair2 = rbt.insert_unique(10);
        EXPECT_FALSE(pair2.second);

        rbt.insert_unique(7);
        rbt.insert_unique(8);

        rbt.insert_unique(15);
        rbt.insert_unique(5);
        rbt.insert_unique(6);

        rbt.insert_unique(11);
        rbt.insert_unique(13);
        rbt.insert_unique(12);

        EXPECT_EQ(rbt.size(), 9);
        EXPECT_EQ(dump(rbt), "5 6 7 8 10 11 12 13 15");

        // verify tree structure by matching each path from root to leaf node
        std::vector<std::string> ans = {
            "8 6 5",
            "8 6 7",
            "8 11 10",
            "8 11 13 12",
            "8 11 13 15",
        };
        auto paths = rbt.get_root_to_leaf_paths();
        EXPECT_TRUE(std::equal(paths.begin(), paths.end(), ans.begin()));

        // find
        EXPECT_EQ(*rbt.find(8), 8);
        EXPECT_EQ(rbt.find(9), rbt.end());

        // modify
        auto ret = rbt.find(8);
        *ret = 999;
        // note: tree's properties may be affected: no longer sorted
        EXPECT_EQ(dump(rbt), "5 6 7 999 10 11 12 13 15");
    }
}
