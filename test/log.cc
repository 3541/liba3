#include <cerrno>
#include <cstdio>
#include <sstream>

#include <gtest/gtest.h>

#include <a3/log.h>
#include <a3/str.h>

class LogTest : public ::testing::Test {
    static constexpr size_t READ_BUF_SIZE = 512;

protected:
    FILE* stream { nullptr };

    void SetUp() override {
        stream = tmpfile();
        a3_log_init(stream, LOG_DEBUG);
    }

    void TearDown() override { fclose(stream); }

    std::string read_written() const {
        fflush(stream);
        fseek(stream, 0, SEEK_SET);
        std::string ret(READ_BUF_SIZE, '\0');
        size_t      written = fread(&ret[0], sizeof(char), READ_BUF_SIZE, stream);
        ret.resize(written);
        return ret;
    }
};

TEST_F(LogTest, init) {
    ASSERT_TRUE(stream);
    fputc('c', stream);
    ASSERT_EQ(read_written(), "c");
}

TEST_F(LogTest, msg) {
    a3_log_msg(LOG_DEBUG, "A test message.");
    ASSERT_EQ(read_written(), "A test message.\n");
}

TEST_F(LogTest, filter) {
    a3_log_msg(LOG_TRACE, "This shouldn't appear.");
    ASSERT_EQ(read_written(), "");
}

TEST_F(LogTest, format) {
    a3_log_fmt(LOG_DEBUG, "%d, %0.1f, %s", 123, 1.2, "string");
    ASSERT_EQ(read_written(), "123, 1.2, string\n");
}

TEST_F(LogTest, format_string) {
    a3_log_fmt(LOG_DEBUG, "Some formatting: " A3_S_F, A3_S_FORMAT(A3_CS("test string")));
    ASSERT_EQ(read_written(), "Some formatting: test string\n");
}

TEST_F(LogTest, error) {
    a3_log_error(EINVAL, "An error");
    ASSERT_EQ(read_written(), "Error: An error (Invalid argument).\n");
}

TEST_F(LogTest, macros) {
    int expected_line = __LINE__ + 1;
    A3_ERR("TEST");

    std::stringstream expected;
    expected << __FILE__ << " (" << expected_line << "): TEST\n";

    ASSERT_EQ(read_written(), expected.str().c_str());
}
