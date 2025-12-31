#include "a3/shim/accept.h"

#include <array>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <thread>

#include <gmock/gmock.h>

#include "a3/shim/platform.h"
#include "a3/shim/socket_types.h"
#include "a3/unwrap.h"

#ifdef A3_PLATFORM_OS_WINDOWS
#include <winsock2.h>
#else
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#endif

namespace a3::test {

namespace {

using namespace testing;
using namespace std::chrono_literals;

#ifdef A3_PLATFORM_OS_WINDOWS
#define close_socket   ::closesocket
#define socket_error   WSAGetLastError()
#define SOCKET_BLOCKED WSAEWOULDBLOCK
using SendLength = int;
#else
#define close_socket   ::close
#define socket_error   errno
#define SOCKET_BLOCKED EWOULDBLOCK
using SendLength = std::size_t;
#endif

} // namespace

struct AcceptTest : public Test {
    A3Socket m_fd{0};
    A3Port   m_port;

    AcceptTest() noexcept;
    ~AcceptTest() noexcept override;

    static A3Socket make_socket() noexcept;
};

AcceptTest::AcceptTest() noexcept {
#ifdef A3_PLATFORM_OS_WINDOWS
    WSADATA data;
    A3_UNWRAPSD(WSAStartup(MAKEWORD(2, 2), &data));
#endif

    m_fd = make_socket();
    if (!m_fd)
        return;

    int flag = 1;
    if (::setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&flag),
                     sizeof(flag)) < 0) {
        ADD_FAILURE() << "setsockopt()";
        return;
    }

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_port        = 0;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (::bind(m_fd, reinterpret_cast<sockaddr const*>(&addr), sizeof(addr)) < 0) {
        ADD_FAILURE() << "bind()";
        return;
    }

    if (::listen(m_fd, 1) < 0) {
        ADD_FAILURE() << "listen()";
        return;
    }

    A3Socklen len = sizeof(addr);
    if (::getsockname(m_fd, reinterpret_cast<sockaddr*>(&addr), &len) < 0)
        ADD_FAILURE() << "getsockname()";

    m_port = ntohs(addr.sin_port);
}

AcceptTest::~AcceptTest() noexcept {
    if (m_fd < 0)
        return;

    close_socket(m_fd);
}

A3Socket AcceptTest::make_socket() noexcept {
    auto const s = ::socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
        ADD_FAILURE() << "socket()";

    return s;
}

TEST_F(AcceptTest, no_flags) {
    ASSERT_THAT(m_fd, Gt(0U));
    ASSERT_THAT(m_port, Gt(0U));

    std::string_view message{"Hello, world."};

    std::thread t{[&]() mutable {
        auto const s = make_socket();
        if (s < 0)
            return;

        sockaddr_in addr{};
        addr.sin_family      = AF_INET;
        addr.sin_port        = htons(m_port);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

        if (::connect(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
            ADD_FAILURE() << "connect()";
            return;
        }

        while (!message.empty()) {
            auto const res = ::send(s, message.data(), static_cast<SendLength>(message.size()), 0);
            if (res <= 0)
                break;

            message.remove_prefix(static_cast<std::size_t>(res));
        }

        close_socket(s);
    }};

    auto const a = ::a3_shim_accept(m_fd, nullptr, nullptr, 0);
    ASSERT_THAT(a, Gt(0U));

    std::array<char, 128> buf{'\0'};
    SendLength            written = 0;

    decltype(::recv(0, nullptr, 0, 0)) res = 0;
    do {
        res = ::recv(a, buf.data() + written, static_cast<SendLength>(buf.size()) - written, 0);
        EXPECT_THAT(res, Ge(0));

        if (res < 0)
            break;

        written += static_cast<SendLength>(res);
    } while (res > 0 && static_cast<std::size_t>(written) < buf.size());

    EXPECT_THAT(std::string_view{buf.data()}, Eq("Hello, world."));

    t.join();
    close_socket(a);
}

TEST_F(AcceptTest, nonblock) {
    ASSERT_THAT(m_fd, Gt(0U));
    ASSERT_THAT(m_port, Gt(0U));

    std::string_view message{"Hello, world."};
    std::atomic_bool ready{false};

    std::thread t{[&]() mutable {
        auto const s = make_socket();
        if (s < 0)
            return;

        sockaddr_in addr{};
        addr.sin_family      = AF_INET;
        addr.sin_port        = htons(m_port);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

        if (::connect(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
            ADD_FAILURE() << "connect()";
            return;
        }

        while (!ready.load())
            std::this_thread::sleep_for(0.5s);

        while (!message.empty()) {
            auto const res = ::send(s, message.data(), static_cast<SendLength>(message.size()), 0);
            if (res <= 0)
                break;

            message.remove_prefix(static_cast<std::size_t>(res));
        }

        close_socket(s);
    }};

    auto const a = ::a3_shim_accept(m_fd, nullptr, nullptr, A3_SOCK_NONBLOCK);
    ASSERT_THAT(a, Gt(0U));

    std::array<char, 128> buf{'\0'};
    SendLength            written = 0;

    EXPECT_THAT(::recv(a, buf.data(), 1, 0), Lt(0));
    EXPECT_THAT(socket_error, Eq(SOCKET_BLOCKED));

    ready.store(true);

    while (static_cast<std::size_t>(written) < buf.size()) {
        auto const res =
            ::recv(a, buf.data() + written, static_cast<SendLength>(buf.size()) - written, 0);

        if (res > 0) {
            written += static_cast<SendLength>(res);
            continue;
        }

        if (res == 0)
            break;
        ASSERT_THAT(res > 0 || socket_error == SOCKET_BLOCKED, IsTrue());

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(a, &fds);
        ::timeval tv{.tv_sec = 2, .tv_usec = 0};

        ASSERT_THAT(select(static_cast<int>(a) + 1, &fds, nullptr, nullptr, &tv), Gt(0));
        ASSERT_THAT(FD_ISSET(a, &fds), IsTrue());
    }

    EXPECT_THAT(std::string_view{buf.data()}, Eq("Hello, world."));

    t.join();
    close_socket(a);
}

} // namespace a3::test
