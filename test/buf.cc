#include <gtest/gtest.h>

#include <a3/buffer.h>
#include <a3/str.h>

#define BUF_INITIAL_CAP (128ULL)
#define BUF_MAX_CAP     (512ULL)

class BufferTest : public ::testing::Test {
protected:
    A3Buffer buf { A3_S_NULL, 0, 0, 0 }; // NOLINT(misc-non-private-member-variables-in-classes)

    void SetUp() override { a3_buf_init(&buf, BUF_INITIAL_CAP, BUF_MAX_CAP); }

    void TearDown() override { a3_buf_destroy(&buf); }
};

TEST_F(BufferTest, init) { ASSERT_TRUE(a3_buf_initialized(&buf)); }

TEST_F(BufferTest, cursor) {
    ASSERT_EQ(buf.head, 0ULL);
    ASSERT_EQ(buf.tail, 0ULL);

    a3_buf_write_byte(&buf, 'b');
    EXPECT_EQ(buf.head, 0ULL);
    EXPECT_EQ(buf.tail, 1ULL);

    a3_buf_read(&buf, 1);
    EXPECT_EQ(buf.head, 0ULL);
    EXPECT_EQ(buf.tail, 0ULL);
}

TEST_F(BufferTest, write_byte) {
    a3_buf_write_byte(&buf, 'b');
    ASSERT_EQ(*buf.data.ptr, 'b');
}

TEST_F(BufferTest, write_line) {
    a3_buf_write_line(&buf, A3_CS("some text"));
    ASSERT_EQ(a3_string_cmpi(a3_buf_read_ptr(&buf), A3_CS("some text\n")), 0);
}

TEST_F(BufferTest, write_fmt) {
    a3_buf_write_fmt(&buf, "%d %0.1f %s", 123, 1.23, "string");
    ASSERT_EQ(a3_string_cmpi(a3_buf_read_ptr(&buf), A3_CS("123 1.2 string")), 0);
}

TEST_F(BufferTest, write_num) {
    a3_buf_write_num(&buf, 1234567);
    ASSERT_EQ(a3_string_cmpi(a3_buf_read_ptr(&buf), A3_CS("1234567")), 0);
}

TEST_F(BufferTest, write_struct) {
    struct TestStruct {
        int32_t a;
        int32_t b;
    };
    static_assert(sizeof(TestStruct) == 8ULL, "Unexpected size.");

    TestStruct t = { 42, 43 };
    ASSERT_EQ(t.a, 42);
    ASSERT_EQ(t.b, 43);
    A3_BUF_WRITE_STRUCT(&buf, t);

    EXPECT_EQ(*reinterpret_cast<int32_t*>(buf.data.ptr), 42);
    EXPECT_EQ(reinterpret_cast<int32_t*>(buf.data.ptr)[1], 43);
}

TEST_F(BufferTest, reset) {
    a3_buf_write_line(&buf, A3_CS("A line"));
    ASSERT_NE(buf.tail, 0ULL);

    a3_buf_reset_if_empty(&buf);
    EXPECT_NE(buf.tail, 0ULL);

    a3_buf_read(&buf, sizeof("A line\n") - 1);
    a3_buf_reset_if_empty(&buf);
    EXPECT_EQ(buf.tail, 0ULL);
}

TEST_F(BufferTest, compact) {
    a3_buf_write_str(&buf, A3_CS("some string"));
    ASSERT_EQ(buf.head, 0ULL);

    a3_buf_read(&buf, 2);
    EXPECT_NE(buf.head, 0ULL);
    size_t tail = buf.tail;

    a3_buf_compact(&buf);
    EXPECT_EQ(buf.head, 0ULL);
    EXPECT_NE(buf.tail, tail);
}

TEST_F(BufferTest, memmem) {
    a3_buf_write_str(&buf, A3_CS("A string with a needle in it."));

    A3String found = a3_buf_memmem(&buf, A3_CS("needle"));
    ASSERT_TRUE(found.ptr);
    EXPECT_EQ(found.ptr, &buf.data.ptr[16]);
    EXPECT_EQ(found.len, sizeof("needle") - 1);

    found = a3_buf_memmem(&buf, A3_CS("not in there"));
    ASSERT_FALSE(found.ptr);
}

TEST_F(BufferTest, tokenization) {
    a3_buf_write_str(&buf, A3_CS("A string with a number of tokens in it."));

    A3String s = a3_buf_token_next(&buf, A3_CS(" "), A3_PRES_END_NO);
    EXPECT_TRUE(s.ptr);
    EXPECT_STREQ(a3_string_cstr(A3_S_CONST(s)), "A");
    EXPECT_EQ(buf.head, 2ULL);
    EXPECT_EQ(s.ptr[s.len], '\0');

    s = a3_buf_token_next(&buf, A3_CS(" "), A3_PRES_END_YES);
    EXPECT_TRUE(s.ptr);
    EXPECT_EQ(buf.head, 8ULL);
    EXPECT_EQ(s.ptr[s.len], ' ');

    A3String scmp = a3_string_alloc(s.len + 1);
    a3_string_copy(scmp, A3_S_CONST(s));
    scmp.ptr[scmp.len - 1] = '\0';
    EXPECT_STREQ(a3_string_cstr(A3_S_CONST(scmp)), "string");
    a3_string_free(&scmp);

    s = a3_buf_token_next_copy(&buf, A3_CS(" "), A3_PRES_END_NO);
    EXPECT_EQ(a3_string_cmp(A3_S_CONST(s), A3_CS("with")), 0);
    a3_string_free(&s);
}

TEST_F(BufferTest, grow) {
    ASSERT_EQ(buf.data.len, BUF_INITIAL_CAP);

    for (uint16_t i = 0; i < 256; i++) {
        ASSERT_TRUE(a3_buf_write_byte(&buf, (uint8_t)i));
    }

    EXPECT_NE(buf.data.len, BUF_INITIAL_CAP);
    EXPECT_LE(buf.data.len, BUF_MAX_CAP);

    bool failed = false;
    for (uint16_t i = 0; i < BUF_MAX_CAP; i++) {
        if ((failed = !a3_buf_write_byte(&buf, (uint8_t)i)))
            break;
    }
    EXPECT_TRUE(failed);
    EXPECT_EQ(buf.data.len, BUF_MAX_CAP);
}
