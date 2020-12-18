#include <gtest/gtest.h>

#include <a3/ht.h>
#include <a3/str.h>

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
    EXPECT_EQ(HT_SIZE(CString, CString)(&table), 0);
    EXPECT_EQ(table.cap, HT_INITIAL_CAP);
    EXPECT_TRUE(table.entries);
}

TEST_F(HTTest, insert_and_delete) {
    EXPECT_EQ(HT_SIZE(CString, CString)(&table), 0);

    HT_INSERT(CString, CString)(&table, CS("A key"), CS("A value"));
    EXPECT_EQ(HT_SIZE(CString, CString)(&table), 1);
    EXPECT_EQ(string_cmp(*HT_FIND(CString, CString)(&table, CS("A key")),
                         CS("A value")),
              0);

    EXPECT_TRUE(HT_DELETE(CString, CString)(&table, CS("A key")));
    EXPECT_EQ(HT_SIZE(CString, CString)(&table), 0);
    EXPECT_FALSE(HT_FIND(CString, CString)(&table, CS("A key")));
}
