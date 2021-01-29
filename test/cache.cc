#include <vector>

#include <gtest/gtest.h>

#include <a3/cache.h>
#include <a3/log.h>
#include <a3/platform/types_private.h>
#include <a3/str.h>

using std::vector;

A3_CACHE_DEFINE_STRUCTS(A3CString, A3CString)

A3_CACHE_DECLARE_METHODS(A3CString, A3CString)
A3_CACHE_DEFINE_METHODS_NOHT(A3CString, A3CString)

class CacheTest : public ::testing::Test {
protected:
    static constexpr size_t CACHE_CAPACITY = 512;
    A3_CACHE(A3CString, A3CString) cache {};

    void SetUp() override {
        a3_log_init(stderr, LOG_DEBUG);
        A3_CACHE_INIT(A3CString, A3CString)(&cache, CACHE_CAPACITY, nullptr);
    }

    void TearDown() override { A3_CACHE_DESTROY(A3CString, A3CString)(&cache); }
};

TEST_F(CacheTest, init) {
    EXPECT_TRUE(cache.accessed);
    EXPECT_EQ(cache.table.size, 0ULL);
    EXPECT_EQ(cache.table.cap, CACHE_CAPACITY);
    EXPECT_TRUE(cache.table.entries);
}

TEST_F(CacheTest, insert) {
    A3_CACHE_INSERT(A3CString, A3CString)
    (&cache, A3_CS("Key"), A3_CS("Value"), nullptr);

    auto* found = A3_CACHE_FIND(A3CString, A3CString)(&cache, A3_CS("Key"));
    ASSERT_TRUE(found);

    ssize_t index = A3_HT_FIND_INDEX(A3CString, A3CString)(&cache.table, A3_CS("Key"));
    ASSERT_GE(index, 0LL);
    ASSERT_EQ(found, &cache.table.entries[(size_t)index].value);
    EXPECT_TRUE(A3_CACHE_ACCESSED(A3CString, A3CString)(&cache, (size_t)index));
}

TEST_F(CacheTest, eviction) {
    vector<A3String> strings;

    for (size_t i = 0; i < CACHE_CAPACITY * 3; i++) {
        auto s = a3_string_itoa(i);
        strings.push_back(s);
        auto sc = A3_S_CONST(s);
        A3_CACHE_INSERT(A3CString, A3CString)(&cache, sc, sc, nullptr);
        ASSERT_TRUE(A3_CACHE_FIND(A3CString, A3CString)(&cache, sc));
        EXPECT_LE(cache.table.size, cache.table.cap);
        EXPECT_EQ(cache.table.cap, CACHE_CAPACITY);
    }

    for (auto& s : strings)
        a3_string_free(&s);
}

static size_t evicted = 0;
static void   eviction_callback(void* ctx, A3CString* key, A3CString* value) {
    (void)ctx;
    (void)value;
    evicted++;
    a3_string_free(reinterpret_cast<A3String*>(key));
}

TEST_F(CacheTest, eviction_callback) {
    A3_CACHE_DESTROY(A3CString, A3CString)(&cache);
    A3_CACHE_INIT(A3CString, A3CString)
    (&cache, CACHE_CAPACITY, eviction_callback);

    for (size_t i = 0; i < CACHE_CAPACITY * 2; i++) {
        auto s = A3_S_CONST(a3_string_itoa(i));
        A3_CACHE_INSERT(A3CString, A3CString)(&cache, s, s, nullptr);
    }

    EXPECT_EQ(evicted, CACHE_CAPACITY);
    EXPECT_EQ(cache.table.size, CACHE_CAPACITY);

    for (size_t i = 0; i < CACHE_CAPACITY; i++)
        A3_CACHE_EVICT(A3CString, A3CString)(&cache, nullptr);

    EXPECT_EQ(evicted, CACHE_CAPACITY * 2);
    EXPECT_EQ(cache.table.size, 0ULL);
}
