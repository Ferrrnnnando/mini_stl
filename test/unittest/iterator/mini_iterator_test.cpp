#include "mini_stl/test/mini_unittest.h"

#include "mini_stl/container/mini_container_vector.h"
#include "mini_stl/iterator/mini_iterator.h"
#include "mini_stl/utility/mini_utility.h"

namespace mini {

TEST(mini_iterator_adapter_test, back_insert_iterator)
{
    ctnr::vector<int> vec;
    EXPECT_EQ(vec.size(), 0);

    auto back_inserter = iter::back_inserter(vec);

    back_inserter = 1;
    EXPECT_EQ(dump(vec), "1");
    back_inserter = 2;
    EXPECT_EQ(dump(vec), "1 2");
}

}  // namespace mini
