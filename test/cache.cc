#include <vector>

#include <gtest/gtest.h>

#include <a3/cache.h>
#include <a3/log.h>
#include <a3/platform/types_private.h>
#include <a3/str.h>

using std::vector;

CACHE_DEFINE_STRUCTS(CString, CString)

CACHE_DECLARE_METHODS(CString, CString)
CACHE_DEFINE_METHODS_NOHT(CString, CString)

class CacheTest : public ::testing::Test {
protected:
    static constexpr size_t CACHE_CAPACITY = 512;
    CACHE(CString, CString) cache {};

    void SetUp() override {
        log_init(stderr, DEBUG);
        CACHE_INIT(CString, CString)(&cache, CACHE_CAPACITY, nullptr);
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
    CACHE_INSERT(CString, CString)(&cache, CS("Key"), CS("Value"), nullptr);

    auto* found = CACHE_FIND(CString, CString)(&cache, CS("Key"));
    ASSERT_TRUE(found);

    ssize_t index = HT_FIND_INDEX(CString, CString)(&cache.table, CS("Key"));
    ASSERT_GE(index, 0LL);
    ASSERT_EQ(found, &cache.table.entries[(size_t)index].value);
    EXPECT_TRUE(CACHE_ACCESSED(CString, CString)(&cache, (size_t)index));
}

TEST_F(CacheTest, eviction) {
    vector<String> strings;

    for (size_t i = 0; i < CACHE_CAPACITY * 3; i++) {
        auto s = string_itoa(i);
        strings.push_back(s);
        auto sc = S_CONST(s);
        CACHE_INSERT(CString, CString)(&cache, sc, sc, nullptr);
        ASSERT_TRUE(CACHE_FIND(CString, CString)(&cache, sc));
        EXPECT_LE(cache.table.size, cache.table.cap);
        EXPECT_EQ(cache.table.cap, CACHE_CAPACITY);
    }

    for (auto& s : strings)
        string_free(&s);
}

static size_t evicted = 0;
static void eviction_callback(void* ctx, CString* key, CString* value) {
    (void)ctx;
    (void)value;
    evicted++;
    string_free(reinterpret_cast<String*>(key));
}

TEST_F(CacheTest, eviction_callback) {
    CACHE_DESTROY(CString, CString)(&cache);
    CACHE_INIT(CString, CString)(&cache, CACHE_CAPACITY, eviction_callback);

    for (size_t i = 0; i < CACHE_CAPACITY * 2; i++) {
        auto s = S_CONST(string_itoa(i));
        CACHE_INSERT(CString, CString)(&cache, s, s, nullptr);
    }

    EXPECT_EQ(evicted, CACHE_CAPACITY);
    EXPECT_EQ(cache.table.size, CACHE_CAPACITY);

    for (size_t i = 0; i < CACHE_CAPACITY; i++)
        CACHE_EVICT(CString, CString)(&cache, nullptr);

    EXPECT_EQ(evicted, CACHE_CAPACITY * 2);
    EXPECT_EQ(cache.table.size, 0ULL);
}
