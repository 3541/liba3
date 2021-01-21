#include <gtest/gtest.h>

#include <a3/str.h>

TEST(String, construct) {
    A3CString lit = A3_CS("test");
    EXPECT_STREQ(A3_S_AS_C_STR(lit), "test");
    EXPECT_EQ(lit.len, 4ULL);

    A3CString s = A3_CS_OF("test");
    EXPECT_STREQ(A3_S_AS_C_STR(s), "test");
    EXPECT_EQ(s.len, 4ULL);

    EXPECT_FALSE(A3_S_AS_C_STR(A3_CS_NULL));
}

TEST(String, alloc) {
    A3String s = a3_string_alloc(4);
    EXPECT_TRUE(s.ptr);
    EXPECT_EQ(s.len, 4ULL);

    // string_free should clear pointer and length.
    a3_string_free(&s);
    EXPECT_FALSE(s.ptr);
    EXPECT_EQ(s.len, 0ULL);
}

TEST(String, realloc) {
    A3String orig = a3_string_clone(A3_CS("str123"));
    ASSERT_EQ(a3_string_cmp(A3_S_CONST(orig), A3_CS("str123")), 0);

    A3String next = a3_string_realloc(&orig, orig.len + 2);
    EXPECT_FALSE(orig.ptr);
    EXPECT_EQ(orig.len, 0ULL);
    next.ptr[next.len - 2] = '\0';
    EXPECT_STREQ(A3_S_AS_C_STR(A3_S_CONST(next)), "str123");
    EXPECT_EQ(next.len, 8ULL);

    a3_string_free(&next);
}

TEST(String, clone) {
    // Should return null if given a null string.
    A3String s = a3_string_clone(A3_CS_NULL);
    EXPECT_FALSE(s.ptr);
    EXPECT_EQ(s.len, 0ULL);

    s = a3_string_clone(A3_CS("test string"));
    EXPECT_TRUE(s.ptr);
    EXPECT_EQ(s.len, sizeof("test string") - 1);
    EXPECT_EQ(a3_string_cmp(A3_S_CONST(s), A3_CS("test string")), 0);

    a3_string_free(&s);
}

TEST(String, copy) {
    A3CString s1 = A3_CS("test str1");
    A3String  s2 = a3_string_clone(A3_CS("Longer test string."));

    // Copy from/to null Strings does nothing.
    a3_string_copy(s2, A3_CS_NULL);
    EXPECT_EQ(a3_string_cmp(A3_S_CONST(s2), A3_CS("Longer test string.")), 0);
    a3_string_copy(A3_CS_MUT(A3_CS_NULL),
                   A3_S_CONST(s2)); // This would segfault otherwise.

    a3_string_copy(s2, s1);
    s2.ptr[s1.len] = '\0'; // So string comparison works.
    EXPECT_EQ(s2.len, sizeof("Longer test string.") - 1);
    EXPECT_STREQ(A3_S_AS_C_STR(A3_S_CONST(s2)), "test str1");
    a3_string_free(&s2);
    s2 = a3_string_clone(A3_CS("An even longer string."));

    A3String s3 = a3_string_alloc(s1.len + 1);
    // Copy of a larger string into a smaller one is truncated.
    a3_string_copy(s3, A3_S_CONST(s2));
    s3.ptr[s3.len - 1] = '\0';
    EXPECT_STREQ(A3_S_AS_C_STR(A3_S_CONST(s3)), "An even l");

    a3_string_free(&s2);
    a3_string_free(&s3);
}

TEST(String, concat) {
    A3String dst = a3_string_alloc(11);

    a3_string_concat(dst, 5, A3_CS("01"), A3_CS("23"), A3_CS("45"), A3_CS("67"),
                     A3_CS("89"));
    dst.ptr[dst.len - 1] = '\0';
    EXPECT_STREQ(A3_S_AS_C_STR(A3_S_CONST(dst)), "0123456789");

    a3_string_free(&dst);
}

TEST(String, isascii) {
    EXPECT_TRUE(a3_string_isascii(A3_CS("ASCII string")));

    A3String not_ascii = a3_string_clone(A3_CS("some data"));
    not_ascii.ptr[2]   = 0;
    not_ascii.ptr[3]   = 255;
    EXPECT_FALSE(a3_string_isascii(A3_S_CONST(not_ascii)));

    a3_string_free(&not_ascii);
}

TEST(String, compare) {
    EXPECT_EQ(a3_string_cmp(A3_CS("same"), A3_CS("same")), 0);
    EXPECT_EQ(a3_string_cmpi(A3_CS("same"), A3_CS("same")), 0);
    EXPECT_NE(a3_string_cmp(A3_CS("cAsE"), A3_CS("CaSe")), 0);
    EXPECT_EQ(a3_string_cmpi(A3_CS("cAsE"), A3_CS("CaSe")), 0);
    EXPECT_NE(a3_string_cmp(A3_CS("s1"), A3_CS("s2")), 0);
    EXPECT_NE(a3_string_cmpi(A3_CS("s1"), A3_CS("s2")), 0);
}

TEST(String, rchr) {
    A3CString t   = A3_CS("12345654321");
    A3CString ans = { .ptr = &t.ptr[7], .len = 4 };

    A3CString got = a3_string_rchr(t, '4');
    EXPECT_EQ(ans.ptr, got.ptr);
    EXPECT_EQ(ans.len, got.len);
}
