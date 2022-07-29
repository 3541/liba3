#include <cstddef>
#include <cstdlib>
#include <unordered_map>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <a3/ht.h>
#include <a3/str.h>
#include <a3/util.h>
#include <a3/util.hh>

#include <gmock/gmock-more-matchers.h>

A3_HT_DEFINE_STRUCTS(A3CString, A3CString)

A3_HT_DECLARE_METHODS(A3CString, A3CString)
A3_HT_DEFINE_METHODS(A3CString, A3CString, a3_string_cptr, a3_string_len, a3_string_cmp)

namespace a3 {
namespace test {
namespace ht {

using std::vector;
using namespace testing;

class HTTest : public Test {
    A3_PINNED(HTTest);

protected:
    A3_HT(A3CString, A3CString) table {}; // NOLINT(misc-non-private-member-variables-in-classes)

    HTTest() { A3_HT_INIT(A3CString, A3CString)(&table, A3_HT_NO_HASH_KEY, A3_HT_ALLOW_GROWTH); }
    ~HTTest() { A3_HT_DESTROY(A3CString, A3CString)(&table); }
};

TEST_F(HTTest, init) {
    EXPECT_EQ(A3_HT_SIZE(A3CString, A3CString)(&table), 0ULL);
    EXPECT_EQ(table.cap, A3_HT_INITIAL_CAP);
    EXPECT_TRUE(table.entries);
}

TEST_F(HTTest, insert_and_delete) {
    EXPECT_EQ(A3_HT_SIZE(A3CString, A3CString)(&table), 0ULL);

    A3_HT_INSERT(A3CString, A3CString)
    (&table, A3_CS("A key"), A3_CS("A value"));
    EXPECT_EQ(A3_HT_SIZE(A3CString, A3CString)(&table), 1ULL);
    EXPECT_EQ(
        a3_string_cmp(*A3_HT_FIND(A3CString, A3CString)(&table, A3_CS("A key")), A3_CS("A value")),
        0);

    EXPECT_TRUE(A3_HT_DELETE(A3CString, A3CString)(&table, A3_CS("A key")));
    EXPECT_EQ(A3_HT_SIZE(A3CString, A3CString)(&table), 0ULL);
    EXPECT_FALSE(A3_HT_FIND(A3CString, A3CString)(&table, A3_CS("A key")));
}

TEST_F(HTTest, grow) {
    vector<A3CString> keys;

    auto all_present = [this, &keys]() {
        for (auto& key : keys) {
            auto* value = A3_HT_FIND(A3CString, A3CString)(&table, key);
            ASSERT_TRUE(value);
            EXPECT_EQ(a3_string_cmp(key, *value), 0);
        }
    };

    for (size_t i = 0; i < A3_HT_INITIAL_CAP * 4; i++) {
        A3CString s = A3_S_CONST(a3_string_itoa(i));
        A3_HT_INSERT(A3CString, A3CString)(&table, s, s);
        EXPECT_TRUE(A3_HT_FIND(A3CString, A3CString)(&table, s));
        keys.push_back(s);
    }

    EXPECT_EQ(A3_HT_SIZE(A3CString, A3CString)(&table), A3_HT_INITIAL_CAP * 4);
    EXPECT_GE(table.cap, A3_HT_INITIAL_CAP * 4);
    all_present();

    while (!keys.empty()) {
        // NOLINTNEXTLINE(concurrency-mt-unsafe, cert-msc30-c, cert-msc50-cpp)
        auto key = keys.begin() + rand() % static_cast<int>(keys.size());
        A3_HT_DELETE(A3CString, A3CString)(&table, *key);
        A3String tmp = A3_CS_MUT(*key);
        a3_string_free(&tmp);
        keys.erase(key);

        all_present();
    }

    EXPECT_EQ(A3_HT_SIZE(A3CString, A3CString)(&table), 0ULL);
}

// This test is deliberately meant to provoke the issue discovered in 4f33b27.
TEST_F(HTTest, fixed_size) {
    constexpr size_t TEST_CAP = 512;
    A3_HT_RESIZE(A3CString, A3CString)(&table, TEST_CAP);

    vector<A3String> keys;

    table.can_grow = false;

    // Generate keys.
    for (size_t i = 0; i < TEST_CAP; i++)
        keys.push_back(a3_string_itoa(i));

    // Fill every slot.
    auto fill_table = [this, &keys]() -> bool {
        for (auto& key : keys) {
            auto key_const = A3_S_CONST(key);
            A3_HT_INSERT(A3CString, A3CString)(&table, key_const, key_const);
            A3_TRYB(A3_HT_FIND(A3CString, A3CString)(&table, key_const));
        }

        return true;
    };

    // Clear the whole table. Every slot should now be a tombstone.
    auto clear_table = [this, &keys]() {
        for (auto& key : keys)
            ASSERT_TRUE(A3_HT_DELETE(A3CString, A3CString)(&table, A3_S_CONST(key)));

        EXPECT_EQ(table.size, 0ULL);
    };

    for (size_t i = 1; i <= 50; i++) {
        ASSERT_TRUE(fill_table());
        clear_table();
    }

    for (auto& key : keys)
        a3_string_free(&key);
}

TEST_F(HTTest, duplicate_reject) {
    EXPECT_TRUE(A3_HT_INSERT(A3CString, A3CString)(&table, A3_CS("key"), A3_CS("val1")));
    EXPECT_TRUE(A3_HT_FIND(A3CString, A3CString)(&table, A3_CS("key")));

    EXPECT_FALSE(A3_HT_INSERT(A3CString, A3CString)(&table, A3_CS("key"), A3_CS("val2")));
    EXPECT_EQ(a3_string_cmp(*A3_HT_FIND(A3CString, A3CString)(&table, A3_CS("key")), A3_CS("val1")),
              0);
}

static bool combine_val(A3CString* current_value, A3CString new_value) {
    A3String combined_value = a3_string_alloc(current_value->len + new_value.len + 2);
    a3_string_concat(combined_value, 3, *current_value, A3_CS(", "), new_value);
    *current_value = A3_S_CONST(combined_value);
    return true;
}

TEST_F(HTTest, duplicate_combine) {
    A3_HT_SET_DUPLICATE_CB(A3CString, A3CString)(&table, combine_val);

    EXPECT_TRUE(A3_HT_INSERT(A3CString, A3CString)(&table, A3_CS("key"), A3_CS("val1")));
    EXPECT_TRUE(A3_HT_FIND(A3CString, A3CString)(&table, A3_CS("key")));

    EXPECT_TRUE(A3_HT_INSERT(A3CString, A3CString)(&table, A3_CS("key"), A3_CS("val2")));
    A3CString* combined_value = A3_HT_FIND(A3CString, A3CString)(&table, A3_CS("key"));
    EXPECT_EQ(a3_string_cmp(*combined_value, A3_CS("val1, val2")), 0);

    a3_string_free(reinterpret_cast<A3String*>(combined_value));
}

TEST_F(HTTest, for_each) {
    std::unordered_map<A3CString, A3String> strings;

    for (unsigned i = 1; i <= 128; i++) {
        A3String str = a3_string_clone(a3_cstring_from(std::to_string(i).data()));

        strings.insert({ str, str });
        A3_HT_INSERT(A3CString, A3CString)(&table, str, str);
    }

    A3_HT_FOR_EACH (A3CString, A3CString, &table, k, v) {
        auto entry = strings.find(*k);
        EXPECT_THAT(entry, Ne(strings.end()));
        if (entry == strings.end())
            break;

        EXPECT_THAT(entry->second, Eq(*v));
        if (entry->second != *v)
            break;

        A3String s = entry->second;
        strings.erase(entry->first);
        a3_string_free(&s);
    }

    EXPECT_THAT(strings.size(), IsFalse()) << "FOR_EACH did not yield all elements.";
}

TEST_F(HTTest, nested_for_each) {
    std::unordered_map<A3CString, A3String> strings;

    for (unsigned i = 1; i <= 128; i++) {
        A3String str = a3_string_clone(a3_cstring_from(std::to_string(i).data()));

        strings.insert({ str, str });
        A3_HT_INSERT(A3CString, A3CString)(&table, str, str);
    }

    size_t count = 0;
    A3_HT_FOR_EACH (A3CString, A3CString, &table, k, v) {
        auto entry = strings.find(*k);
        EXPECT_THAT(entry, Ne(strings.end()));
        if (entry == strings.end())
            break;

        EXPECT_THAT(entry->second, Eq(*v));
        if (entry->second != *v)
            break;

        A3_HT_FOR_EACH (A3CString, A3CString, &table, k_inner, v_inner) {
            auto entry_inner = strings.find(*k_inner);
            EXPECT_THAT(entry_inner, Ne(strings.end()));
            if (entry_inner == strings.end())
                break;

            EXPECT_THAT(entry_inner->second, Eq(*v_inner));
            if (entry_inner->second != *v_inner)
                break;
        }

        count++;
    }

    EXPECT_THAT(count, Eq(128ULL)) << "FOR_EACH did not yield all elements.";

    for (auto entry : strings)
        a3_string_free(&entry.second);
}

} // namespace ht
} // namespace test
} // namespace a3
