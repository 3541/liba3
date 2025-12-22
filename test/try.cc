#include "a3/try.h"

#include <gtest/gtest.h>

#ifdef _MSC_VER
// Disable warning about constant arguments to conditional expressions.
#pragma warning(disable : 4127)
#endif

namespace a3::test::try_macros {

TEST(Try, TRY) {
    EXPECT_FALSE([]() {
        A3_TRY(false);
        return true;
    }());

    EXPECT_TRUE([]() {
        A3_TRY(true);
        return true;
    }());

    EXPECT_EQ(
        []() {
            A3_TRY(false, -42);
            return 42;
        }(),
        -42);

    EXPECT_EQ(
        []() {
            A3_TRY(true, -42);
            return 42;
        }(),
        42);
}

} // namespace a3::test::try_macros
