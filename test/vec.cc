#include <cstdlib>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <a3/vec.h>

namespace a3 {
namespace test {
namespace vec {

using namespace testing;

class VecTest : public Test { // NOLINT(cppcoreguidelines-special-member-functions)
protected:
    A3Vec vec; // NOLINT(misc-non-private-member-variables-in-classes)

    ~VecTest() { a3_vec_destroy(&vec); }
};

TEST_F(VecTest, simple) {
    A3_VEC_INIT(size_t, &vec);

    for (size_t i = 0; i < 400; i++)
        A3_VEC_PUSH(&vec, &i);

    EXPECT_THAT(vec.len, Eq(400ULL));
    EXPECT_THAT(vec.cap, Gt(400ULL));

    for (size_t i = 0; i < 400; i++) {
        auto* v = A3_VEC_AT(size_t, &vec, i);
        EXPECT_THAT(v, NotNull());
        if (!v)
            break;

        EXPECT_THAT(*v, Eq(i));
        if (*v != i)
            break;
    }
}

TEST_F(VecTest, for_each) {
    A3_VEC_INIT(size_t, &vec);

    for (size_t i = 0; i < 400; i++)
        A3_VEC_PUSH(&vec, &i);

    EXPECT_THAT(vec.len, Eq(400ULL));
    EXPECT_THAT(vec.cap, Gt(400ULL));

    size_t j = 0;
    A3_VEC_FOR_EACH (size_t, i, &vec) {
        EXPECT_THAT(*i, Eq(j++));
        if (*i != j - 1)
            break;
    }
}

TEST_F(VecTest, alignment) {
    struct alignas(64) WeirdAlignment {
        size_t a; // NOLINT(misc-non-private-member-variables-in-classes)
        size_t b; // NOLINT(misc-non-private-member-variables-in-classes)

        bool operator==(WeirdAlignment const& other) const { return other.a == a && other.b == b; }
        bool operator!=(WeirdAlignment const& other) const { return !(*this == other); }
    };

    A3_VEC_INIT(WeirdAlignment, &vec);
    std::vector<WeirdAlignment> v;

    for (size_t i = 0; i < 400; i++) {
        // NOLINTNEXTLINE(cert-msc50-cpp, cert-msc30-c, concurrency-mt-unsafe)
        WeirdAlignment w { (size_t)rand(), (size_t)rand() };

        v.push_back(w);
        A3_VEC_PUSH(&vec, &w);
    }

    while (!v.empty()) {
        auto  actual   = A3_VEC_POP(WeirdAlignment, &vec);
        auto& expected = v.back();

        EXPECT_EQ(actual, expected);
        if (actual != expected)
            break;

        v.pop_back();
    }
}

TEST_F(VecTest, nested_for_each) {
    A3_VEC_INIT(unsigned, &vec);

    for (unsigned i = 1; i <= 128; i++)
        A3_VEC_PUSH(&vec, &i);

    unsigned outer = 1;
    A3_VEC_FOR_EACH (unsigned, i, &vec) {
        EXPECT_THAT(*i, Eq(outer++));

        unsigned inner = 1;
        A3_VEC_FOR_EACH (unsigned, j, &vec)
            EXPECT_THAT(*j, Eq(inner++));
    }
}

} // namespace vec
} // namespace test
} // namespace a3
