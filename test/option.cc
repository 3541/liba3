#include <string>
#include <string_view>
#include <utility>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#ifdef __cpp_concepts
#include <a3/option.hh>

namespace a3::test::option {

using namespace a3;
using namespace testing;

TEST(Option, map) {
    Option<std::string> victim{"Hello"};
    auto                victim1 = std::move(victim).map([](auto v) { return v + ", world."; });

    EXPECT_THAT(victim1.has_value(), IsTrue());
    EXPECT_THAT(victim1.value(), StrEq("Hello, world."));
}

TEST(Option, map_ref) {
    Option<std::string> base{"Hello, world."};
    auto                victim = base.as_ref().map([](auto& v) { return std::string_view{v}; });

    EXPECT_THAT(victim.has_value(), IsTrue());
    EXPECT_THAT(victim.value(), StrEq("Hello, world."));

    EXPECT_THAT(victim.as_ref().map_or("o no", [](auto& v) { return v; }), StrEq("Hello, world."));
    EXPECT_THAT(Option<std::string>{}.map_or("o no", [](auto v) { return v; }), StrEq("o no"));
}

} // namespace a3::test::option

#endif
