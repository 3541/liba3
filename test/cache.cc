#include <gtest/gtest.h>

#include <a3/cache.h>
#include <a3/str.h>

CACHE_DEFINE_STRUCTS(CString, CString)

CACHE_DECLARE_METHODS(CString, CString)
CACHE_DEFINE_METHODS_NOHT(CString, CString)

class CacheTest : public ::testing::Test {
protected:
    static constexpr size_t CACHE_CAPACITY = 512;
    CACHE(CString, CString) cache {};

    void SetUp() override {
        CACHE_INIT(CString, CString)(&cache, CACHE_CAPACITY);
    }

    void TearDown() override { CACHE_DESTROY(CString, CString)(&cache); }
};

TEST_F(CacheTest, init) {
    EXPECT_TRUE(cache.accessed);
    EXPECT_EQ(cache.table.size, 0ULL);
    EXPECT_EQ(cache.table.cap, CACHE_CAPACITY);
    EXPECT_TRUE(cache.table.entries);
}

TEST_F(CacheTest, insert) {
    CACHE_INSERT(CString, CString)(&cache, CS("Key"), CS("Value"));

    auto* found = CACHE_FIND(CString, CString)(&cache, CS("Key"));
    ASSERT_TRUE(found);
    size_t index =
        (size_t)(CONTAINER_OF(found, HT_ENTRY(CString, CString), value) -
                 cache.table.entries) /
        sizeof(HT_ENTRY(CString, CString));
    EXPECT_TRUE(CACHE_ACCESSED(CString, CString)(&cache, index));
}
