#include <gtest/gtest.h>

#include <a3/util.h>

TEST(Try, TRYB) {
    EXPECT_FALSE([]() {
        A3_TRYB(false);
        return true;
    }());

    EXPECT_TRUE([]() {
        A3_TRYB(true);
        return true;
    }());
}

TEST(Try, TRYB_MAP) {
    EXPECT_EQ(
        []() {
            A3_TRYB_MAP(false, -42);
            return 42;
        }(),
        -42);

    EXPECT_EQ(
        []() {
            A3_TRYB_MAP(true, -42);
            return 42;
        }(),
        42);
}

TEST(Try, TRY_COND) {
    EXPECT_TRUE([]() {
        A3_TRY_COND(-1, < 0, true);
        return false;
    }());

    EXPECT_FALSE([]() {
        A3_TRY_COND(42, < 0, true);
        return false;
    }());
}

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

    EXPECT_TRUE([]() {
        A3_TRY(-1, < 0, true);
        return false;
    }());

    EXPECT_FALSE([]() {
        A3_TRY(42, < 0, true);
        return false;
    }());
}
