#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

#include <gtest/gtest.h>

#include <a3/pool.h>

namespace a3 {
namespace test {
namespace pool {

using std::vector;

// An object to store in the pool.
struct TestObject {
    size_t f1 {};
    size_t f2 {};
    char   f3[10] {};
};

class PoolTest : public ::testing::Test {
    static constexpr size_t POOL_SIZE = 1024;

protected:
    A3Pool* pool { nullptr }; // NOLINT(misc-non-private-member-variables-in-classes)

    void SetUp() override {
        pool = A3_POOL_OF(TestObject, POOL_SIZE, A3_POOL_ZERO_BLOCKS, nullptr, nullptr);
    }

    void TearDown() override { a3_pool_free(pool); }

    TestObject* alloc() { return static_cast<TestObject*>(a3_pool_alloc_block(pool)); }

    void free(TestObject* block) { a3_pool_free_block(pool, static_cast<void*>(block)); }

    void* pool_start() const { return *reinterpret_cast<void**>(pool); }

    void* pool_end() const {
        return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(pool_start()) +
                                       POOL_SIZE * (sizeof(TestObject) + sizeof(void*)));
    }
};

TEST_F(PoolTest, init) {
    ASSERT_TRUE(pool);
    ASSERT_TRUE(*reinterpret_cast<void**>(pool));
}

TEST_F(PoolTest, alloc) {
    auto* block = alloc();
    ASSERT_TRUE(block);

    void* block_ptr = static_cast<void*>(block);

    EXPECT_LE(pool_start(), block_ptr);
    EXPECT_LT(block_ptr, pool_end());

    EXPECT_EQ(block->f1, 0ULL);
    EXPECT_EQ(block->f2, 0ULL);
    EXPECT_STREQ(block->f3, "");

    block->f1 = 123;
    block->f2 = 456;
    strcpy(block->f3, "string");
    EXPECT_EQ(block->f1, 123ULL);
    EXPECT_EQ(block->f2, 456ULL);
    EXPECT_STREQ(block->f3, "string");
}

TEST_F(PoolTest, free) {
    auto* block = alloc();
    ASSERT_TRUE(block);

    free(block);

    // Now, the block should have a next pointer to another block somewhere.
    auto* next = *reinterpret_cast<void**>(block + 1);
    EXPECT_LE(pool_start(), next);
    EXPECT_LT(next, pool_end());
}

TEST_F(PoolTest, alloc_all) {
    vector<TestObject*> allocations;

    // Alloc the entire pool.
    while (auto* a = alloc()) {
        // There should be no duplicate pointers.
        EXPECT_EQ(std::find(allocations.begin(), allocations.end(), a), allocations.end());
        allocations.push_back(a);
    }

    // Can't alloc anymore.
    EXPECT_FALSE(alloc());

    size_t count = allocations.size();
    for (auto* a : allocations) {
        free(a);
    }
    allocations.clear();

    // Now, it should be possible to reallocate that many objects.
    while (auto* a = alloc()) {
        allocations.push_back(a);
    }
    EXPECT_EQ(allocations.size(), count);
}

} // namespace pool
} // namespace test
} // namespace a3
