#ifdef __cpp_concepts
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <a3/result.hh>

using namespace a3;
using namespace testing;

TEST(Result, constructible_from_t) {
    Result<int, int> victim { 42 };
    EXPECT_THAT(std::move(victim).unwrap(), Eq(42));
}

TEST(Result, constructible_from_err_e) {
    Result<int, int> victim { Err { 42 } };
    EXPECT_THAT(std::move(victim).unwrap_err(), Eq(42));
}

TEST(Result, constructible_from_convertible_to_t) {
    Result<std::string, int> victim { "Hello" };
    EXPECT_THAT(std::move(victim).unwrap(), StrEq("Hello"));
}

TEST(Result, constructible_from_err_convertible_to_e) {
    Result<int, std::string> victim { Err { "Hello" } };
    EXPECT_THAT(std::move(victim).unwrap_err(), StrEq("Hello"));
}

TEST(Result, as_ref) {
    Result<std::string, size_t> victim { "Hello" };

    auto victim_ref = victim.as_ref();
    EXPECT_THAT(victim.is_ok(), IsTrue());
    EXPECT_THAT(victim_ref.is_ok(), IsTrue());
    EXPECT_THAT(static_cast<void*>(&victim_ref.unwrap()), Eq(static_cast<void*>(&victim)));
}

TEST(Result, copy_constructible) {
    Result<std::string, size_t> victim { "Hello" };
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    Result victim1 { victim };

    EXPECT_THAT(victim.is_ok(), IsTrue());
    EXPECT_THAT(victim1.is_ok(), IsTrue());
    EXPECT_THAT(std::move(victim).unwrap(), StrEq(std::move(victim1).unwrap()));

    Result<size_t, std::string> victim2 { Err { "o no" } };
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    Result victim3 { victim2 };

    EXPECT_THAT(victim2.is_err(), IsTrue());
    EXPECT_THAT(victim3.is_err(), IsTrue());
    EXPECT_THAT(std::move(victim2).unwrap_err(), StrEq(std::move(victim3).unwrap_err()));
}

TEST(Result, copy_assignable) {
    Result<std::string, size_t> victim { "Hello" };
    Result<std::string, size_t> victim1 { "world" };

    EXPECT_THAT(victim.is_ok(), IsTrue());
    EXPECT_THAT(victim1.is_ok(), IsTrue());
    EXPECT_THAT(victim.as_ref().unwrap(), StrNe(victim1.as_ref().unwrap()));

    victim = victim1;

    EXPECT_THAT(victim.is_ok(), IsTrue());
    EXPECT_THAT(victim1.is_ok(), IsTrue());
    EXPECT_THAT(std::move(victim).unwrap(), StrEq(std::move(victim1).unwrap()));

    Result<size_t, std::string> victim2 { Err { "o" } };
    Result<size_t, std::string> victim3 { Err { "no" } };

    EXPECT_THAT(victim2.is_err(), IsTrue());
    EXPECT_THAT(victim3.is_err(), IsTrue());
    EXPECT_THAT(victim2.as_ref().unwrap_err(), StrNe(victim3.as_ref().unwrap_err()));

    victim2 = victim3;

    EXPECT_THAT(victim2.is_err(), IsTrue());
    EXPECT_THAT(victim3.is_err(), IsTrue());
    EXPECT_THAT(std::move(victim2).unwrap_err(), StrEq(std::move(victim3).unwrap_err()));
}

TEST(Result, move_constructible) {
    Result<std::string, size_t> victim { "Hello" };
    EXPECT_THAT(victim.is_ok(), IsTrue());

    Result victim1 { std::move(victim) };
    EXPECT_THAT(victim.is_ok(), IsFalse());  // NOLINT(bugprone-use-after-move)
    EXPECT_THAT(victim.is_err(), IsFalse()); // NOLINT(bugprone-use-after-move)
    EXPECT_THAT(victim1.is_ok(), IsTrue());
    EXPECT_THAT(std::move(victim1).unwrap(), StrEq("Hello"));

    Result<size_t, std::string> victim2 { Err { "o no" } };
    EXPECT_THAT(victim2.is_err(), IsTrue());

    Result victim3 { std::move(victim2) };
    EXPECT_THAT(victim2.is_ok(), IsFalse());  // NOLINT(bugprone-use-after-move)
    EXPECT_THAT(victim2.is_err(), IsFalse()); // NOLINT(bugprone-use-after-move)
    EXPECT_THAT(victim3.is_err(), IsTrue());
    EXPECT_THAT(std::move(victim3).unwrap_err(), StrEq("o no"));
}

TEST(Result, return_ok) {
    auto victim = []() -> Result<std::string, size_t> { return "Working"; }();
    EXPECT_THAT(std::move(victim).unwrap(), StrEq("Working"));
}

TEST(Result, return_err) {
    auto victim = []() -> Result<size_t, std::string> { return Err { "Bad" }; }();
    EXPECT_THAT(std::move(victim).unwrap_err(), Eq("Bad"));
}

#ifdef A3_RTRY
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
    EXPECT_THAT(std::move(result).unwrap(), StrEq("4242"));

    auto result1 = process(true);
    EXPECT_THAT(result1.is_err(), IsTrue());
    EXPECT_THAT(std::move(result1).unwrap_err(), StrEq("o no"));
}
#endif

TEST(Result, map) {
    auto fallible = [](bool fail) -> Result<int, std::string> {
        if (fail)
            return Err { "o no" };
        return 42;
    };

    EXPECT_THAT(fallible(false).map([](auto v) { return v * 2; }).unwrap(), Eq(84));
    EXPECT_THAT(fallible(true).map([](auto v) { return v * 2; }).unwrap_err(), StrEq("o no"));
}

TEST(Result, as_deref) {
    Result<std::unique_ptr<int>, size_t> victim { std::make_unique<int>(1234) };
    auto                                 victim_deref = victim.as_deref();

    EXPECT_THAT(victim_deref.is_ok(), IsTrue());
    EXPECT_THAT(victim.is_ok(), IsTrue());
    EXPECT_THAT(victim_deref.unwrap(), Eq(*victim.as_ref().unwrap()));
}

TEST(Result, unwrap_or) {
    Result<int, int> victim { 42 };
    EXPECT_THAT(victim.unwrap_or(32), Eq(42));

    victim = Err { 26 };
    EXPECT_THAT(victim.unwrap_or(32), Eq(32));
}

TEST(Result, unwrap_or_else) {
    Result<std::string, int> victim { "Hello" };
    EXPECT_THAT(std::move(victim).unwrap_or_else([] { return "o no"; }), StrEq("Hello"));

    Result<std::string, int> victim1 { Err { 42 } };
    EXPECT_THAT(std::move(victim1).unwrap_or_else([] { return "o no"; }), StrEq("o no"));
}

TEST(Result, map_err) {
    auto fallible = [](bool fail) -> Result<int, std::string> {
        if (fail)
            return Err { "o no" };
        return 42;
    };

    EXPECT_THAT(fallible(false).map_err([](auto e) { return e + "!"; }).unwrap(), Eq(42));
    EXPECT_THAT(fallible(true).map_err([](auto e) { return e + "!"; }).unwrap_err(),
                StrEq("o no!"));
}

#endif
