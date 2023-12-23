#ifndef MINI_UNITTEST_H
#define MINI_UNITTEST_H

#include "mini_stl/test/mini_test.h"

#include <sstream>

template<typename Container>
std::string dump(const Container& c)
{
    std::stringstream ss;
    for (const auto& e : c) {
        ss << e << " ";
    }
    return ss.str().substr(0, ss.str().size() - 1);
};

class mini_unittest : public ::testing::Test {
public:
    mini_unittest() = default;

    ~mini_unittest() noexcept override = default;

    void TestBody() override {}

    MINI_DISALLOW_COPY_AND_MOVE(mini_unittest);
};

#endif
