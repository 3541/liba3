#if __cplusplus >= 201703L

#include <string>
#include <string_view>
#include <utility>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <a3/option.hh>

namespace a3::test::option {

using namespace a3;
using namespace testing;

TEST(Option, map) {
    Option<std::string> victim { "Hello" };
    auto                victim1 = std::move(victim).map([](auto v) { return v + ", world."; });

    EXPECT_THAT(victim1.has_value(), IsTrue());
    EXPECT_THAT(victim1.value(), StrEq("Hello, world."));

    auto victim2 = victim1.as_ref().map([](auto& v) { return std::string_view { v }; });
    EXPECT_THAT(victim2.has_value(), IsTrue());
    EXPECT_THAT(victim2.value(), StrEq(victim1.as_ref().value()));

    EXPECT_THAT(victim2.as_ref().map_or("o no", [](auto& v) { return v; }), StrEq("Hello, world."));
    EXPECT_THAT(Option<std::string> {}.map_or("o no", [](auto v) { return v; }), StrEq("o no"));
}

#ifdef __cpp_concepts
TEST(Option, result_conversion) {
    Option<std::string> victim { "Hello" };

    auto res = victim.as_ref().ok_or(-1);
    EXPECT_THAT(res.is_ok(), IsTrue());
    EXPECT_THAT(res.unwrap(), StrEq("Hello"));

    Option<std::string> victim1 {};
    auto                res1 = victim1.as_ref().ok_or(-1);
    EXPECT_THAT(res1.is_err(), IsTrue());
    EXPECT_THAT(res1.unwrap_err(), Eq(-1));
}
#endif

} // namespace a3::test::option

#endif
