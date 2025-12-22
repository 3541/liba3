#include "a3/shim/memmem.h"

#include <gmock/gmock.h>

namespace a3::test {

using namespace testing;

struct MemmemTest : public Test {
protected:
    std::vector<std::byte> m_buf;

    MemmemTest();
};

MemmemTest::MemmemTest() {
    m_buf.reserve('z' - 'a' + 1);

    for (std::underlying_type_t<std::byte> c = 'a'; c <= 'z'; ++c)
        m_buf.push_back(std::byte{c});
}

TEST_F(MemmemTest, missing) {
    EXPECT_THAT(::a3_shim_memmem(m_buf.data(), m_buf.size(), "asdf", 4), IsNull());
}

TEST_F(MemmemTest, present) {
    auto const* res = ::a3_shim_memmem(m_buf.data(), m_buf.size(), "xyz", 3);
    EXPECT_THAT(res, Eq(&m_buf[m_buf.size() - 3]));
    ASSERT_THAT(res, NotNull());
    EXPECT_THAT(*static_cast<char const*>(res), Eq('x'));
}

TEST_F(MemmemTest, multiple) {
    m_buf.push_back(std::byte{'a'});
    m_buf.push_back(std::byte{'b'});

    auto const* res = ::a3_shim_memmem(m_buf.data(), m_buf.size(), "ab", 2);
    EXPECT_THAT(res, Eq(m_buf.data()));
    EXPECT_THAT(*static_cast<char const*>(res), Eq('a'));
}

} // namespace a3::test
