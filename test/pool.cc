#include <algorithm>
#include <vector>

#include <gtest/gtest.h>

#include <a3/pool.h>

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
    Pool* pool { nullptr };

    void SetUp() override { pool = pool_new(sizeof(TestObject), POOL_SIZE); }

    void TearDown() override { pool_free(pool); }

    TestObject* alloc() {
        return static_cast<TestObject*>(pool_alloc_block(pool));
    }

    void free(TestObject* block) {
        pool_free_block(pool, static_cast<void*>(block));
    }

    void* pool_start() const { return *reinterpret_cast<void**>(pool); }

    void* pool_end() const {
        return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(pool_start()) +
                                       POOL_SIZE * sizeof(TestObject));
    }
};

TEST_F(PoolTest, init) {
    ASSERT_TRUE(pool);
    ASSERT_TRUE(*reinterpret_cast<void**>(pool));
}

TEST_F(PoolTest, alloc) {
    auto block = alloc();
    ASSERT_TRUE(block);

    void* block_ptr = static_cast<void*>(block);

    EXPECT_LE(pool_start(), block_ptr);
    EXPECT_LT(block_ptr, pool_end());

    EXPECT_EQ(block->f1, 0);
    EXPECT_EQ(block->f2, 0);
    EXPECT_STREQ(block->f3, "");

    block->f1 = 123;
    block->f2 = 456;
    strcpy(block->f3, "string");
    EXPECT_EQ(block->f1, 123);
    EXPECT_EQ(block->f2, 456);
    EXPECT_STREQ(block->f3, "string");
}

TEST_F(PoolTest, free) {
    auto block = alloc();
    ASSERT_TRUE(block);

    free(block);

    // Now, the block should have a next pointer to another block somewhere.
    auto next = *reinterpret_cast<void**>(block);
    EXPECT_LE(pool_start(), next);
    EXPECT_LT(next, pool_end());
}

TEST_F(PoolTest, alloc_all) {
    vector<TestObject*> allocations;

    // Alloc the entire pool.
    while (auto a = alloc()) {
        // There should be no duplicate pointers.
        EXPECT_EQ(std::find(allocations.begin(), allocations.end(), a),
                  allocations.end());
        allocations.push_back(a);
    }

    // Can't alloc anymore.
    EXPECT_FALSE(alloc());

    size_t count = allocations.size();
    for (auto a : allocations) {
        free(a);
    }
    allocations.clear();

    // Now, it should be possible to reallocate that many objects.
    while (auto a = alloc()) {
        allocations.push_back(a);
    }
    EXPECT_EQ(allocations.size(), count);
}
