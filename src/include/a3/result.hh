/*
 * RESULT -- Sum type error handling.
 *
 * Copyright (c) 2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 *
 * This is an implementation of the concept expressed by std::expected, though it is more similar in
 * spirit to Rust's Result. This file requires C++20.
 */

#pragma once

#ifndef __cpp_concepts
#warn "Result requires C++20 concepts"
#else

#include <concepts>
#include <cstdint>
#include <utility>

#include <a3/util.h>

namespace a3 {

template <typename E>
class Err {
private:
    E m_err;

public:
    template <typename F>
    // NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
    requires(std::constructible_from<E, F>) explicit Err(F&& err) :
        m_err { std::forward<F>(err) } {}

    template <typename F>
    requires(std::constructible_from<E, F>) explicit Err(Err<F>&& err) :
        m_err { std::forward<Err<F>>(err).err() } {}

    E err() const& { return m_err; }

    E err() && { return std::move(m_err); }
};

template <typename E>
Err(E&&) -> Err<E>;

template <typename T, typename E>
class Result {
private:
    enum class State : uint8_t { Ok, Err, MovedFrom };

    union {
        T      m_ok;
        Err<E> m_err;
    };
    State m_state;

public:
    template <typename U>
    requires(std::constructible_from<T, U> && !std::same_as<Err<E>, U> &&
             // NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
             !std::same_as<Result<T, E>, U>) Result(U&& value) :
        m_ok { std::forward<U>(value) }, m_state { State::Ok } {}

    template <typename F>
    requires(std::constructible_from<E, F>) Result(Err<F>&& err) :
        m_err { std::forward<Err<F>>(err) }, m_state { State::Err } {}

    // TODO: Add variants of copy and move constructors and assignment operators for trivial types.
    Result(Result const& other) : m_state { other.m_state } {
        switch (m_state) {
        case State::Ok:
            new (&m_ok) T { other.m_ok };
            break;
        case State::Err:
            new (&m_err) Err { other.m_err };
            break;
        case State::MovedFrom:
            break;
        }
    }

    Result& operator=(Result const& other) {
        if (this == &other)
            return *this;

        this->~Result();

        m_state = other.m_state;
        switch (m_state) {
        case State::Ok:
            new (&m_ok) T { other.m_ok };
            break;
        case State::Err:
            new (&m_err) Err { other.m_err };
            break;
        case State::MovedFrom:
            break;
        }

        return *this;
    }

    Result(Result&& other) noexcept : m_state { other.m_state } {
        other.m_state = State::MovedFrom;
        switch (m_state) {
        case State::Ok:
            new (&m_ok) T { std::move(other.m_ok) };
            break;
        case State::Err:
            new (&m_err) Err { std::move(other.m_err) };
            break;
        case State::MovedFrom:
            break;
        }
    }

    Result& operator=(Result&& other) noexcept {
        this->~Result();

        m_state       = other.m_state;
        other.m_state = State::MovedFrom;
        switch (m_state) {
        case State::Ok:
            new (&m_ok) T { std::move(other.m_ok) };
            break;
        case State::Err:
            new (&m_err) Err { std::move(other.m_err) };
            break;
        case State::MovedFrom:
            break;
        }

        return *this;
    }

    // TODO: Add trivial destructor.
    ~Result() {
        switch (m_state) {
        case State::Ok:
            if constexpr (!std::is_trivially_destructible_v<T>)
                m_ok.~T();
            break;
        case State::Err:
            if constexpr (!std::is_trivially_destructible_v<Err<E>>)
                m_err.~Err();
            break;
        case State::MovedFrom:
            break;
        }
    }

    bool is_ok() const { return m_state == State::Ok; }
    bool is_err() const { return m_state == State::Err; }

    T unwrap() const& {
        A3_UNWRAPND(is_ok());
        return m_ok;
    }

    T unwrap() && {
        A3_UNWRAPND(is_ok());
        m_state = State::MovedFrom;
        return std::move(m_ok);
    }

    E unwrap_err() const& {
        A3_UNWRAPND(!is_ok());
        return m_err.err();
    }

    E unwrap_err() && {
        A3_UNWRAPND(!is_ok());
        m_state = State::MovedFrom;
        return std::move(m_err).err();
    }
};

#if defined(__GNUC__) || defined(__clang__)
#define A3_RTRY(R)                                                                                 \
    ({                                                                                             \
        auto _tmp = (R);                                                                           \
        if (_tmp.is_err())                                                                         \
            return Err { std::move(_tmp).unwrap_err() };                                           \
        else if (!_tmp.is_ok())                                                                    \
            A3_PANIC("A3_RTRY on moved-from Result.");                                             \
        std::move(_tmp).unwrap();                                                                  \
    })
#endif

} // namespace a3

#endif
