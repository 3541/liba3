#include <cerrno>
#include <cstdio>
#include <sstream>

#include <gtest/gtest.h>

#include <a3/log.h>

class LogTest : public ::testing::Test {
protected:
    FILE* stream { nullptr };
    char* buf { nullptr };
    size_t buf_size { 0 };

    void SetUp() override {
        stream = open_memstream(&buf, &buf_size);
        log_init(stream, DEBUG);
    }

    void TearDown() override {
        fclose(stream);
        free(buf);
    }
};

TEST_F(LogTest, init) {
    ASSERT_TRUE(stream);
    fputc('c', stream);
    fflush(stream);
    ASSERT_STREQ(buf, "c");
}

TEST_F(LogTest, msg) {
    log_msg(DEBUG, "A test message.");
    fflush(stream);

    ASSERT_STREQ(buf, "A test message.\n");
}

TEST_F(LogTest, filter) {
    log_msg(TRACE, "This shouldn't appear.");
    fflush(stream);

    ASSERT_STREQ(buf, "");
}

TEST_F(LogTest, format) {
    log_fmt(DEBUG, "%d, %0.1f, %s", 123, 1.2, "string");
    fflush(stream);

    ASSERT_STREQ(buf, "123, 1.2, string\n");
}

TEST_F(LogTest, error) {
    log_error(EINVAL, "An error");
    fflush(stream);

    ASSERT_STREQ(buf, "Error: An error (Invalid argument).\n");
}

TEST_F(LogTest, macros) {
    int expected_line = __LINE__ + 2;

    ERR("TEST");
    fflush(stream);

    std::stringstream expected;
    expected << __FILE__ << " (" << expected_line << "): TEST\n";

    ASSERT_STREQ(buf, expected.str().c_str());
}
