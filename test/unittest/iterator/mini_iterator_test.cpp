#include "mini_stl/test/mini_unittest.h"

#include "mini_stl/algorithm/mini_algorithm_base.h"
#include "mini_stl/container/mini_container_list.h"
#include "mini_stl/iterator/mini_iterator_list.h"

using namespace std;

TEST(mini_iterator, basic)
{
    using value_type = int;
    using item_type = mini::container::ListItem<value_type>;
    using group_type = mini::container::List<value_type>;
    using iterator_type = mini::iterator::ListIter<item_type>;
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
    // iter = min::algo::find<>(begin, end, 3);
    if (iter == end) {
        cout << "not found" << endl;
    } else {
        cout << "val " << iter->value() << ": found" << endl;
    }
    EXPECT_EQ(iter->value(), 3);

    iter = find_func(begin, end, 7);
    if (iter == end) {
        cout << "not found" << endl;
    } else {
        cout << "val " << iter->value() << ": found" << endl;
    }
}
