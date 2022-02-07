#include <cerrno>
#include <cstdio>
#include <sstream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define A3_LOG_LEVEL A3_LOG_DEBUG
#include <a3/log.h>
#include <a3/str.h>

using namespace testing;

class LogTest : public Test {
    static constexpr size_t READ_BUF_SIZE = 512;

protected:
    FILE* stream { nullptr }; // NOLINT(misc-non-private-member-variables-in-classes)

    void SetUp() override {
        stream = tmpfile();
        a3_log_init(stream);
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
    EXPECT_EQ(read_written(), "c");
}

TEST_F(LogTest, msg) {
    A3_DEBUG("A test message.");
    EXPECT_THAT(read_written(), HasSubstr("A test message.\n"));
}

TEST_F(LogTest, filter) {
    A3_TRACE("This shouldn't appear.");
    EXPECT_THAT(read_written(), IsEmpty());
}

TEST_F(LogTest, format) {
    A3_DEBUG_F("%d, %0.1f, %s", 123, 1.2, "string");
    EXPECT_THAT(read_written(), HasSubstr("123, 1.2, string\n"));
}

TEST_F(LogTest, format_string) {
    A3_DEBUG_F("Some formatting: " A3_S_F, A3_S_FORMAT(A3_CS("test string")));
    EXPECT_THAT(read_written(), HasSubstr("Some formatting: test string\n"));
}

TEST_F(LogTest, error) {
    A3_ERRNO(EINVAL, "An error");

    EXPECT_THAT(read_written(), HasSubstr(strerror(EINVAL))); // NOLINT(concurrency-mt-unsafe)
}

TEST_F(LogTest, macros) {
    int expected_line = __LINE__ + 1;
    A3_ERROR("TEST");

    std::stringstream expected;
    expected << __FILE__ << " (" << expected_line << "): TEST\n";

    ASSERT_EQ(read_written(), expected.str().c_str());
}
