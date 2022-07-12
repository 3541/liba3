#include <cstdlib>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <a3/vec.h>

namespace a3 {
namespace test {
namespace vec {

using namespace testing;

class VecTest : public Test {
protected:
    A3Vec vec; // NOLINT(misc-non-private-member-variables-in-classes)

    ~VecTest() { a3_vec_destroy(&vec); }
};

TEST_F(VecTest, simple) {
    A3_VEC_INIT(size_t, &vec);

    for (size_t i = 0; i < 400; i++)
        A3_VEC_PUSH(&vec, &i);

    EXPECT_THAT(vec.len, Eq(400));
    EXPECT_THAT(vec.cap, Gt(400));

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

    EXPECT_THAT(vec.len, Eq(400));
    EXPECT_THAT(vec.cap, Gt(400));

    size_t j = 0;
    A3_VEC_FOR_EACH(size_t, i, &vec) {
        EXPECT_THAT(*i, Eq(j++));
        if (*i != j - 1)
            break;
    }
}

TEST_F(VecTest, alignment) {
    struct alignas(64) WeirdAlignment {
        size_t a;
        size_t b;

        bool operator==(WeirdAlignment const& other) const { return other.a == a && other.b == b; }
    };

    A3_VEC_INIT(WeirdAlignment, &vec);
    std::vector<WeirdAlignment> v;

    for (size_t i = 0; i < 400; i++) {
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

} // namespace vec
} // namespace test
} // namespace a3
