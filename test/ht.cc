#include <vector>

#include <gtest/gtest.h>

#include <a3/ht.h>
#include <a3/platform_private.h>
#include <a3/str.h>

using std::vector;

HT_DEFINE_STRUCTS(CString, CString)

HT_DECLARE_METHODS(CString, CString)
HT_DEFINE_METHODS(CString, CString, CS_PTR, S_LEN, string_cmp)

class HTTest : public ::testing::Test {
protected:
    HT(CString, CString) table {};

    void SetUp() override { HT_INIT(CString, CString)(&table, true); }

    void TearDown() override { HT_DESTROY(CString, CString)(&table); }
};

TEST_F(HTTest, init) {
    EXPECT_EQ(HT_SIZE(CString, CString)(&table), 0ULL);
    EXPECT_EQ(table.cap, HT_INITIAL_CAP);
    EXPECT_TRUE(table.entries);
}

TEST_F(HTTest, insert_and_delete) {
    EXPECT_EQ(HT_SIZE(CString, CString)(&table), 0ULL);

    HT_INSERT(CString, CString)(&table, CS("A key"), CS("A value"));
    EXPECT_EQ(HT_SIZE(CString, CString)(&table), 1ULL);
    EXPECT_EQ(string_cmp(*HT_FIND(CString, CString)(&table, CS("A key")),
                         CS("A value")),
              0);

    EXPECT_TRUE(HT_DELETE(CString, CString)(&table, CS("A key")));
    EXPECT_EQ(HT_SIZE(CString, CString)(&table), 0ULL);
    EXPECT_FALSE(HT_FIND(CString, CString)(&table, CS("A key")));
}

TEST_F(HTTest, grow) {
    vector<CString> keys;

    auto all_present = [this, &keys]() {
        for (auto& key : keys) {
            auto* value = HT_FIND(CString, CString)(&table, key);
            ASSERT_TRUE(value);
            EXPECT_EQ(string_cmp(key, *value), 0ULL);
        }
    };

    for (size_t i = 0; i < HT_INITIAL_CAP * 4; i++) {
        CString s = S_CONST(string_itoa(i));
        HT_INSERT(CString, CString)(&table, s, s);
        EXPECT_TRUE(HT_FIND(CString, CString)(&table, s));
        keys.push_back(s);
    }

    EXPECT_EQ(HT_SIZE(CString, CString)(&table), HT_INITIAL_CAP * 4);
    EXPECT_GE(table.cap, HT_INITIAL_CAP * 4);
    all_present();

    while (keys.size()) {
        auto key = keys.begin() + rand() % (ssize_t)keys.size();
        HT_DELETE(CString, CString)(&table, *key);
        String tmp = CS_MUT(*key);
        string_free(&tmp);
        keys.erase(key);

        all_present();
    }

    EXPECT_EQ(HT_SIZE(CString, CString)(&table), 0ULL);
}

// This test is deliberately meant to provoke the issue discovered in 4f33b27.
TEST_F(HTTest, fixed_size) {
    constexpr size_t TEST_CAP = 512;
    HT_RESIZE(CString, CString)(&table, TEST_CAP);

    vector<String> keys;

    table.can_grow = false;

    // Generate keys.
    for (size_t i = 0; i < TEST_CAP; i++)
        keys.push_back(string_itoa(i));

    // Fill every slot.
    auto fill_table = [this, &keys]() -> bool {
        for (auto& key : keys) {
            auto key_const = S_CONST(key);
            HT_INSERT(CString, CString)(&table, key_const, key_const);
            TRYB(HT_FIND(CString, CString)(&table, key_const));
        }

        return true;
    };

    // Clear the whole table. Every slot should now be a tombstone.
    auto clear_table = [this, &keys]() {
        for (auto& key : keys)
            ASSERT_TRUE(HT_DELETE(CString, CString)(&table, S_CONST(key)));

        EXPECT_EQ(table.size, 0ULL);
    };

    for (size_t i = 1; i <= 50; i++) {
        ASSERT_TRUE(fill_table());
        clear_table();
    }

    for (auto& key : keys)
        string_free(&key);
}
