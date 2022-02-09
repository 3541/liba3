#ifdef __cpp_concepts
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <a3/result.hh>

using namespace a3;
using namespace testing;

TEST(Result, constructibleFromT) {
    Result<int, int> victim { 42 };
    EXPECT_THAT(victim.unwrap(), Eq(42));
}

TEST(Result, constructibleFromErrE) {
    Result<int, int> victim { Err { 42 } };
    EXPECT_THAT(victim.unwrap_err(), Eq(42));
}

TEST(Result, constructibleFromConvertibleToT) {
    Result<std::string, int> victim { "Hello" };
    EXPECT_THAT(victim.unwrap(), StrEq("Hello"));
}

TEST(Result, constructibleFromErrConvertibleToE) {
    Result<int, std::string> victim { Err { "Hello" } };
    EXPECT_THAT(victim.unwrap_err(), StrEq("Hello"));
}

TEST(Result, copyConstructible) {
    Result<std::string, size_t> victim { "Hello" };
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    Result victim1 { victim };

    EXPECT_THAT(victim.is_ok(), IsTrue());
    EXPECT_THAT(victim1.is_ok(), IsTrue());
    EXPECT_THAT(victim.unwrap(), StrEq(victim1.unwrap()));

    Result<size_t, std::string> victim2 { Err { "o no" } };
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    Result victim3 { victim2 };

    EXPECT_THAT(victim2.is_err(), IsTrue());
    EXPECT_THAT(victim3.is_err(), IsTrue());
    EXPECT_THAT(victim2.unwrap_err(), StrEq(victim3.unwrap_err()));
}

TEST(Result, copyAssignable) {
    Result<std::string, size_t> victim { "Hello" };
    Result<std::string, size_t> victim1 { "world" };

    EXPECT_THAT(victim.is_ok(), IsTrue());
    EXPECT_THAT(victim1.is_ok(), IsTrue());
    EXPECT_THAT(victim.unwrap(), StrNe(victim1.unwrap()));

    victim = victim1;

    EXPECT_THAT(victim.is_ok(), IsTrue());
    EXPECT_THAT(victim1.is_ok(), IsTrue());
    EXPECT_THAT(victim.unwrap(), StrEq(victim1.unwrap()));

    Result<size_t, std::string> victim2 { Err { "o" } };
    Result<size_t, std::string> victim3 { Err { "no" } };

    EXPECT_THAT(victim2.is_err(), IsTrue());
    EXPECT_THAT(victim3.is_err(), IsTrue());
    EXPECT_THAT(victim2.unwrap_err(), StrNe(victim3.unwrap_err()));

    victim2 = victim3;

    EXPECT_THAT(victim2.is_err(), IsTrue());
    EXPECT_THAT(victim3.is_err(), IsTrue());
    EXPECT_THAT(victim2.unwrap_err(), StrEq(victim3.unwrap_err()));
}

TEST(Result, moveConstructible) {
    Result<std::string, size_t> victim { "Hello" };
    EXPECT_THAT(victim.is_ok(), IsTrue());

    Result victim1 { std::move(victim) };
    EXPECT_THAT(victim.is_ok(), IsFalse());
    EXPECT_THAT(victim.is_err(), IsFalse());
    EXPECT_THAT(victim1.is_ok(), IsTrue());
    EXPECT_THAT(victim1.unwrap(), StrEq("Hello"));

    Result<size_t, std::string> victim2 { Err { "o no" } };
    EXPECT_THAT(victim2.is_err(), IsTrue());

    Result victim3 { std::move(victim2) };
    EXPECT_THAT(victim2.is_ok(), IsFalse());
    EXPECT_THAT(victim2.is_err(), IsFalse());
    EXPECT_THAT(victim3.is_err(), IsTrue());
    EXPECT_THAT(victim3.unwrap_err(), StrEq("o no"));
}

TEST(Result, returnOk) {
    auto victim = []() -> Result<std::string, size_t> { return "Working"; }();
    EXPECT_THAT(victim.unwrap(), StrEq("Working"));
}

TEST(Result, returnErr) {
    auto victim = []() -> Result<size_t, std::string> { return Err { "Bad" }; }();
    EXPECT_THAT(victim.unwrap_err(), Eq("Bad"));
}

TEST(Result, try) {
    auto fallible = [](bool fail) -> Result<int, std::string> {
        if (fail)
            return Err { "o no" };
        return 42;
    };

    auto process = [&fallible](bool fail) -> Result<std::string, std::string> {
        auto n = A3_RTRY(fallible(fail));
        return std::to_string(n) + "42";
    };

    auto result = process(false);
    EXPECT_THAT(result.is_ok(), IsTrue());
    EXPECT_THAT(result.unwrap(), StrEq("4242"));

    auto result1 = process(true);
    EXPECT_THAT(result1.is_err(), IsTrue());
    EXPECT_THAT(result1.unwrap_err(), StrEq("o no"));
}

#endif
