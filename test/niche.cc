#include <gtest/gtest.h>

import a3.lang.niche;

namespace a3::test {

TEST(niche, niche) {
    Niche n;
    static_cast<void>(n);
}

} // namespace a3::test
