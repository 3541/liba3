#include <gtest/gtest.h>

#include <a3/buffer.h>

#include <a3/str.h>

class BufferTest : public ::testing::Test {
protected:
    static constexpr size_t BUF_INITIAL_CAP = 128;
    static constexpr size_t BUF_MAX_CAP     = 512;

    Buffer buf { S_NULL, 0, 0, 0 };

    void SetUp() override { buf_init(&buf, BUF_INITIAL_CAP, BUF_MAX_CAP); }

    void TearDown() override { buf_free(&buf); }
};

TEST_F(BufferTest, init) { ASSERT_TRUE(buf_initialized(&buf)); }

TEST_F(BufferTest, cursor) {
    ASSERT_EQ(buf.head, 0ULL);
    ASSERT_EQ(buf.tail, 0ULL);

    buf_write_byte(&buf, 'b');
    EXPECT_EQ(buf.head, 0ULL);
    EXPECT_EQ(buf.tail, 1ULL);

    buf_read(&buf, 1);
    EXPECT_EQ(buf.head, 0ULL);
    EXPECT_EQ(buf.tail, 0ULL);
}

TEST_F(BufferTest, write_byte) {
    buf_write_byte(&buf, 'b');
    ASSERT_EQ(*buf.data.ptr, 'b');
}

TEST_F(BufferTest, write_line) {
    buf_write_line(&buf, CS("some text"));
    ASSERT_EQ(string_cmpi(buf_read_ptr(&buf), CS("some text\n")), 0);
}

TEST_F(BufferTest, write_fmt) {
    buf_write_fmt(&buf, "%d %0.1f %s", 123, 1.23, "string");
    ASSERT_EQ(string_cmpi(buf_read_ptr(&buf), CS("123 1.2 string")), 0);
}

TEST_F(BufferTest, write_num) {
    buf_write_num(&buf, 1234567);
    ASSERT_EQ(string_cmpi(buf_read_ptr(&buf), CS("1234567")), 0);
}

TEST_F(BufferTest, reset) {
    buf_write_line(&buf, CS("A line"));
    ASSERT_NE(buf.tail, 0ULL);

    buf_reset_if_empty(&buf);
    EXPECT_NE(buf.tail, 0ULL);

    buf_read(&buf, sizeof("A line\n") - 1);
    buf_reset_if_empty(&buf);
    EXPECT_EQ(buf.tail, 0ULL);
}

TEST_F(BufferTest, compact) {
    buf_write_str(&buf, CS("some string"));
    ASSERT_EQ(buf.head, 0ULL);

    buf_read(&buf, 2);
    EXPECT_NE(buf.head, 0ULL);
    size_t tail = buf.tail;

    buf_compact(&buf);
    EXPECT_EQ(buf.head, 0ULL);
    EXPECT_NE(buf.tail, tail);
}

TEST_F(BufferTest, memmem) {
    buf_write_str(&buf, CS("A string with a needle in it."));

    String found = buf_memmem(&buf, CS("needle"));
    ASSERT_TRUE(found.ptr);
    EXPECT_EQ(found.ptr, &buf.data.ptr[16]);
    EXPECT_EQ(found.len, sizeof("needle") - 1);

    found = buf_memmem(&buf, CS("not in there"));
    ASSERT_FALSE(found.ptr);
}

TEST_F(BufferTest, tokenization) {
    buf_write_str(&buf, CS("A string with a number of tokens in it."));

    String s = buf_token_next_impl({ &buf, CS(" "), false });
    EXPECT_TRUE(s.ptr);
    EXPECT_STREQ(S_AS_C_STR(S_CONST(s)), "A");
    EXPECT_EQ(buf.head, 2ULL);
    EXPECT_EQ(s.ptr[s.len], '\0');

    s = buf_token_next_impl({ &buf, CS(" "), true });
    EXPECT_TRUE(s.ptr);
    EXPECT_EQ(buf.head, 8ULL);
    EXPECT_EQ(s.ptr[s.len], ' ');

    String scmp = string_alloc(s.len + 1);
    string_copy(scmp, S_CONST(s));
    scmp.ptr[scmp.len - 1] = '\0';
    EXPECT_STREQ(S_AS_C_STR(S_CONST(scmp)), "string");
    string_free(&scmp);

#ifndef _MSC_VER // MSVC doesn't support compound literals.
    s = buf_token_next_copy(&buf, CS(" "));
    EXPECT_STREQ(S_AS_C_STR(S_CONST(s)), "with");
    string_free(&s);
#endif
}

TEST_F(BufferTest, grow) {
    ASSERT_EQ(buf.data.len, BUF_INITIAL_CAP);

    for (uint16_t i = 0; i < 256; i++) {
        ASSERT_TRUE(buf_write_byte(&buf, (uint8_t)i));
    }

    EXPECT_NE(buf.data.len, BUF_INITIAL_CAP);
    EXPECT_LE(buf.data.len, BUF_MAX_CAP);

    bool failed = false;
    for (uint16_t i = 0; i < BUF_MAX_CAP; i++) {
        if ((failed = !buf_write_byte(&buf, (uint8_t)i)))
            break;
    }
    EXPECT_TRUE(failed);
    EXPECT_EQ(buf.data.len, BUF_MAX_CAP);
}
