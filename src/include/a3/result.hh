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
#warning "Result requires C++20 concepts"
#else

#include <concepts>
#include <cstdint>
#include <utility>

#include <a3/util.h>

namespace a3 {

namespace detail {

#ifndef __APPLE__
template <typename T, typename... Args>
concept constructible_from = std::constructible_from<T, Args...>;

template <typename Fn, typename... Args>
concept invocable = std::invocable<Fn, Args...>;
#else
// Apple Clang purports to support concepts, but does not implement these ones from the standard
// library.

template <typename T, typename... Args>
concept constructible_from = std::destructible<T> && std::is_constructible_v<T, Args...>;

template <typename Fn, typename... Args>
concept invocable = requires(Fn&& f, Args&&... args) {
    std::invoke(std::forward<Fn>(f), std::forward<Args>(args)...);
};
#endif

template <typename I>
struct InnerTypeImpl {
    using T                      = I;
    static constexpr bool IS_REF = false;
};

template <typename I>
struct InnerTypeImpl<I&> {
    using T                      = std::reference_wrapper<I>;
    static constexpr bool IS_REF = true;
};

template <typename I>
struct InnerTypeImpl<I const&> {
    using T                      = std::reference_wrapper<I const>;
    static constexpr bool IS_REF = true;
};

template <typename I>
using InnerType = typename InnerTypeImpl<I>::T;

template <typename I>
static constexpr bool IS_REF = InnerTypeImpl<I>::IS_REF;

template <typename T>
concept Deref = requires(T t) {
    *t;
};

template <typename D>
struct DerefTargetImpl;

template <Deref D>
struct DerefTargetImpl<D> {
    using T = decltype(*std::declval<D>());
};

template <typename D>
requires(!Deref<D>) struct DerefTargetImpl<D> {
    using T = std::nullptr_t;
};

template <typename D>
using DerefTarget = typename DerefTargetImpl<D>::T;

} // namespace detail

template <typename E>
class Err {
private:
    using Inner = detail::InnerType<E>;

    Inner m_err;

    template <typename F>
    friend class Err;

public:
    template <typename F>
    // NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
    requires(detail::constructible_from<Inner, F>) explicit Err(F&& err) :
        m_err { std::forward<F>(err) } {}

    template <typename F>
    requires(detail::constructible_from<Inner, typename Err<F>::Inner>) explicit Err(Err<F>&& err) :
        m_err { std::move(err.m_err) } {}

    template <typename F>
    requires(detail::constructible_from<Inner, typename Err<F>::Inner>) explicit Err(
        Err<F> const& err) :
        m_err { err.m_err } {}

    template <typename F>
    requires(detail::IS_REF<E>&& detail::constructible_from<
             Inner, typename Err<F>::Inner const&>) explicit Err(Err<F> const& err) :
        m_err { err.m_err } {}

    template <typename F>
    requires(detail::IS_REF<E>&& detail::constructible_from<
             Inner, typename Err<F>::Inner&>) explicit Err(Err<F>& err) :
        m_err { err.m_err } {}

    E err() const& { return m_err; }

    E err() && { return std::move(m_err); }
};

template <typename E>
Err(E&&) -> Err<E>;

template <typename T, typename E>
class [[nodiscard]] Result {
private:
    using Inner = detail::InnerType<T>;

    enum class State : uint8_t { Ok, Err, MovedFrom };

    template <typename U, typename F>
    friend class Result;

    union {
        detail::InnerType<T> m_ok;
        Err<E>               m_err;
    };
    State m_state;

public:
    template <typename U = T>
    requires(detail::constructible_from<Inner, U> && !std::same_as<Err<E>, U> &&
             // NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
             !std::same_as<Result<T, E>, U>) Result(U&& value) :
        m_ok { std::forward<U>(value) }, m_state { State::Ok } {}

    template <typename F = E>
    requires(detail::constructible_from<E, F>) Result(Err<F>&& err) :
        m_err { std::forward<Err<F>>(err) }, m_state { State::Err } {}

    template <typename U, typename F>
    requires(detail::constructible_from<Inner, typename Result<U, F>::Inner const&>&&
                 detail::constructible_from<E, F const&> &&
             !std::same_as<T, U> && !std::same_as<E, F>) Result(Result<U, F> const& other) :
        m_state { other.m_state } {
        switch (m_state) {
        case State::Ok:
            new (&m_ok) Inner { other.m_ok };
            break;
        case State::Err:
            new (&m_err) Err { other.m_err };
            break;
        case State::MovedFrom:
            break;
        }
    }

    template <typename U, typename F>
    requires(detail::constructible_from<Inner, typename Result<U, F>::Inner&>&&
                 detail::constructible_from<E, F&> &&
             !std::same_as<T, U> && !std::same_as<E, F>) Result(Result<U, F>& other) :
        m_state { other.m_state } {
        switch (m_state) {
        case State::Ok:
            new (&m_ok) Inner { other.m_ok };
            break;
        case State::Err:
            new (&m_err) Err<E> { other.m_err };
            break;
        case State::MovedFrom:
            break;
        }
    }

    template <typename U, typename F>
    requires(detail::constructible_from<Inner, typename Result<U, F>::Inner>&&
                 detail::constructible_from<E, F> &&
             !std::same_as<T, U> && !std::same_as<E, F>) Result(Result<U, F>&& other) :
        m_state { other.m_state } {
        switch (m_state) {
        case State::Ok:
            new (&m_ok) Inner { other.m_ok };
            break;
        case State::Err:
            new (&m_err) Err<E> { other.m_err };
            break;
        case State::MovedFrom:
            break;
        }
    }

    // TODO: Add variants of copy and move constructors and assignment operators for trivial types.
    Result(Result const& other) : m_state { other.m_state } {
        switch (m_state) {
        case State::Ok:
            new (&m_ok) Inner { other.m_ok };
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
            new (&m_ok) Inner { other.m_ok };
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
            new (&m_ok) Inner { std::move(other.m_ok) };
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
            new (&m_ok) Inner { std::move(other.m_ok) };
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
            if constexpr (!std::is_trivially_destructible_v<Inner>)
                m_ok.~Inner();
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

    T unwrap() && {
        A3_UNWRAPND(is_ok());
        m_state = State::MovedFrom;
        return std::move(m_ok);
    }

    T unwrap() const& requires(std::is_trivially_copyable_v<Inner>) {
        A3_UNWRAPND(is_ok());
        return m_ok;
    }

    E unwrap_err() && {
        A3_UNWRAPND(!is_ok());
        m_state = State::MovedFrom;
        return std::move(m_err).err();
    }

    E unwrap_err() const& requires(std::is_trivially_copyable_v<Err<E>>) {
        A3_UNWRAPND(!is_ok());
        return m_err.err();
    }

    template <detail::invocable<T> Fn>
        Result<std::invoke_result_t<Fn, T>, E> map(Fn&& f) && requires(!detail::IS_REF<T>) {
        switch (m_state) {
        case State::Ok:
            return std::forward<Fn>(f)(std::move(m_ok));
        case State::Err:
            return std::move(m_err);
        case State::MovedFrom:
            A3_PANIC("Result::map on moved-from Result.");
        }

        A3_UNREACHABLE();
    }

    template <detail::invocable<T> Fn>
        Result<std::invoke_result_t<Fn, T>, E> map(Fn&& f) && requires(detail::IS_REF<T>) {
        switch (m_state) {
        case State::Ok:
            return std::forward<Fn>(f)(m_ok.get());
        case State::Err:
            return std::move(m_err);
        case State::MovedFrom:
            A3_PANIC("Result::map on moved-from Result.");
        }

        A3_UNREACHABLE();
    }

    Result<T&, E&>             as_ref() & { return *this; }
    Result<T const&, E const&> as_ref() const& { return *this; }

    Result<detail::DerefTarget<T>&, E&> as_deref() & requires detail::Deref<T&> {
        return as_ref().map([](auto& v) -> auto& { return *v; });
    }
    Result<detail::DerefTarget<T> const&, E const&>
    as_deref() const& requires detail::Deref<T const&> {
        return as_ref().map([](auto const& v) -> auto const& { return *v; });
    }
};

#if defined(__GNUC__) || defined(__clang__)
#define A3_RTRY(R)                                                                                 \
    ({                                                                                             \
        auto _tmp = (R);                                                                           \
        if (_tmp.is_err())                                                                         \
            return Err { std::move(_tmp).unwrap_err() };                                           \
        if (!_tmp.is_ok())                                                                         \
            A3_PANIC("A3_RTRY on moved-from Result.");                                             \
        std::move(_tmp).unwrap();                                                                  \
    })
#endif

} // namespace a3

#endif
