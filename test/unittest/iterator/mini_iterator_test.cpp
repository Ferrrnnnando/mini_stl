#include "mini_stl/test/mini_unittest.h"

#include "mini_stl/algorithm/mini_algorithm_base.h"
#include "mini_stl/container/mini_container_list_toy.h"
#include "mini_stl/iterator/mini_iterator_list_toy.h"

TEST(mini_iterator_test_toy, basic)
{
    using value_type = int;
    using item_type = mini::container::ListItem<value_type>;
    using group_type = mini::container::List<value_type>;
    using iterator_type = mini::iter::ListIter<item_type>;
    auto find_func = mini::algo::find<iterator_type, value_type>;

    // List operation
    group_type mylist;
    for (int i = 0; i < 5; i++) {
        mylist.insert_front(i);
        mylist.insert_end(i + 2);
    }
    mylist.display();

    // ListIter
    iterator_type begin(mylist.front());
    iterator_type end;   // defaults ctor to null
    iterator_type iter;  // defaults ctor to null

    iter = find_func(begin, end, 3);
    EXPECT_NE(iter, end);
    EXPECT_EQ(iter->value(), 3);

    iter = find_func(begin, end, 7);
    EXPECT_EQ(iter, end);
    EXPECT_EQ(iter, iterator_type{});
}
