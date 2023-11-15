#ifndef MINI_UNITTEST_H
#define MINI_UNITTEST_H

#include "mini_stl/test/mini_test.h"

class mini_unittest : public ::testing::Test {
public:
    mini_unittest() = default;

    ~mini_unittest() noexcept override = default;

    void TestBody() override {}

    MINI_DISALLOW_COPY_AND_MOVE(mini_unittest);
};

#endif
