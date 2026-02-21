#include "a3/rc.h"

#include <cstddef>

#include <gtest/gtest.h>

namespace a3::test::rc {

struct TestObjectC {
    A3_REFCOUNTED;
};

TEST(Rc, c_interface) {
    auto* o = new TestObjectC;

    A3_REF_INIT(o);
    EXPECT_EQ(A3_REF_COUNT(o), 1U);

    A3_REF(o);
    EXPECT_EQ(A3_REF_COUNT(o), 2U);

    bool d_called = false;
    auto d        = [&d_called](TestObjectC*) { d_called = true; };

    A3_UNREF_D(o, d);
    EXPECT_EQ(A3_REF_COUNT(o), 1U);
    EXPECT_FALSE(d_called);

    A3_UNREF_D(o, d);
    EXPECT_EQ(A3_REF_COUNT(o), 0U);
    EXPECT_TRUE(d_called);

    delete o;
}

} // namespace a3::test::rc
