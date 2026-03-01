#include <gmock/gmock.h>
#include <gtest/gtest.h>

import a3.types.option;

namespace a3::test {

using namespace testing;

TEST(option, simple) {
    Option<int> o{1};

    ASSERT_THAT(o, IsTrue());
    ASSERT_THAT(*o, 1);

    o = {};
    ASSERT_THAT(o, IsFalse());
}

} // namespace a3::test
