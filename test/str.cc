#include <gtest/gtest.h>

#ifndef _MSC_VER
#include <a3/str.h>

TEST(String, construct) {
    CString lit = CS("test");
    EXPECT_STREQ(S_AS_C_STR(lit), "test");
    EXPECT_EQ(lit.len, 4);

    CString s = CS_OF("test");
    EXPECT_STREQ(S_AS_C_STR(s), "test");
    EXPECT_EQ(s.len, 4);

    EXPECT_FALSE(S_AS_C_STR(CS_NULL));
}

TEST(String, alloc) {
    String s = string_alloc(4);
    EXPECT_TRUE(s.ptr);
    EXPECT_EQ(s.len, 4);

    // string_free should clear pointer and length.
    string_free(&s);
    EXPECT_FALSE(s.ptr);
    EXPECT_EQ(s.len, 0);
}

TEST(String, realloc) {
    String orig = string_clone(CS("str123"));
    ASSERT_STREQ(S_AS_C_STR(S_CONST(orig)), "str123");

    String next = string_realloc(&orig, orig.len + 2);
    EXPECT_FALSE(orig.ptr);
    EXPECT_EQ(orig.len, 0);
    EXPECT_STREQ(S_AS_C_STR(S_CONST(next)), "str123");
    EXPECT_EQ(next.len, 8);

    string_free(&next);
}

TEST(String, clone) {
    // Should return null if given a null string.
    String s = string_clone(CS_NULL);
    EXPECT_FALSE(s.ptr);
    EXPECT_EQ(s.len, 0);

    s = string_clone(CS("test string"));
    EXPECT_TRUE(s.ptr);
    EXPECT_EQ(s.len, sizeof("test string") - 1);
    EXPECT_STREQ(S_AS_C_STR(S_CONST(s)), "test string");

    string_free(&s);
}

TEST(String, copy) {
    CString s1 = CS("test str1");
    String  s2 = string_clone(CS("Longer test string."));

    // Copy from/to null Strings does nothing.
    string_copy(s2, CS_NULL);
    EXPECT_STREQ(S_AS_C_STR(S_CONST(s2)), "Longer test string.");
    string_copy(CS_MUT(CS_NULL), S_CONST(s2)); // This would segfault otherwise.

    string_copy(s2, s1);
    s2.ptr[s1.len] = '\0'; // So string comparison works.
    EXPECT_EQ(s2.len, sizeof("Longer test string.") - 1);
    EXPECT_STREQ(S_AS_C_STR(S_CONST(s2)), "test str1");
    string_free(&s2);
    s2 = string_clone(CS("An even longer string."));

    String s3 = string_alloc(s1.len + 1);
    // Copy of a larger string into a smaller one is truncated.
    string_copy(s3, S_CONST(s2));
    s3.ptr[s3.len - 1] = '\0';
    EXPECT_STREQ(S_AS_C_STR(S_CONST(s3)), "An even l");

    string_free(&s2);
    string_free(&s3);
}

TEST(String, concat) {
    String dst = string_alloc(11);

    string_concat(dst, 5, CS("01"), CS("23"), CS("45"), CS("67"), CS("89"));
    dst.ptr[dst.len - 1] = '\0';
    EXPECT_STREQ(S_AS_C_STR(S_CONST(dst)), "0123456789");

    string_free(&dst);
}

TEST(String, isascii) {
    EXPECT_TRUE(string_isascii(CS("ASCII string")));

    String not_ascii = string_clone(CS("some data"));
    not_ascii.ptr[2] = 0;
    not_ascii.ptr[3] = 255;
    EXPECT_FALSE(string_isascii(S_CONST(not_ascii)));

    string_free(&not_ascii);
}

TEST(String, compare) {
    EXPECT_EQ(string_cmpi(CS("same"), CS("same")), 0);
    EXPECT_EQ(string_cmpi(CS("cAsE"), CS("CaSe")), 0);
    EXPECT_NE(string_cmpi(CS("s1"), CS("s2")), 0);
}

TEST(String, rchr) {
    CString t   = CS("12345654321");
    CString ans = { .ptr = &t.ptr[7], .len = 4 };

    CString got = string_rchr(t, '4');
    EXPECT_EQ(ans.ptr, got.ptr);
    EXPECT_EQ(ans.len, got.len);
}

#endif // _MSC_VER
