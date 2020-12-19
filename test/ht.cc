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

    void SetUp() override { HT_INIT(CString, CString)(&table); }

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

    auto all_present = [&]() {
        for (auto& key : keys) {
            auto* value = HT_FIND(CString, CString)(&table, key);
            EXPECT_TRUE(value);
            EXPECT_EQ(string_cmp(key, *value), 0ULL);
        }
    };

    for (size_t i = 0; i < HT_INITIAL_CAP * 4; i++) {
        CString s = S_CONST(string_itoa(i));
        HT_INSERT(CString, CString)(&table, s, s);
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
