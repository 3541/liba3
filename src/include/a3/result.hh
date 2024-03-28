/*
 * RESULT -- Sum type error handling.
 *
 * Copyright (c) 2022, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

/// \file result.hh
/// # Result
/// This is an implementation of the concept expressed by std::expected, though it is more similar
/// in spirit to Rust's Result. This file requires C++20.

#pragma once

#if !defined(__cpp_concepts) && !defined(DOXYGEN)
#warning "Result requires C++20 concepts"
#else

#ifdef DOXYGEN
/// Hack to fix missing C++20 parsing support in Doxygen.
#define requires(...)
#endif

#include <concepts>
#include <cstdint>
#include <functional>
#include <optional>
#include <system_error>
#include <type_traits>
#include <utility>

#include <a3/util.h>

namespace a3 {

#ifndef DOXYGEN
namespace detail {

#ifndef _LIBCPP_VERSION
template <typename T, typename... Args>
concept constructible_from = std::constructible_from<T, Args...>;

template <typename Fn, typename... Args>
concept invocable = std::invocable<Fn, Args...>;

template <typename T>
concept default_initializable = std::default_initializable<T>;

template <typename T>
concept signed_integral = std::signed_integral<T>;
#else
// libc++ purports to support concepts, but does not implement these ones from the standard library.

template <typename T, typename... Args>
concept constructible_from = std::is_destructible_v<T> && std::is_constructible_v<T, Args...>;

template <typename Fn, typename... Args>
concept invocable = requires(Fn&& f, Args&&... args) {
    std::invoke(std::forward<Fn>(f), std::forward<Args>(args)...);
};

template <typename T>
concept default_initializable = constructible_from<T> && requires {
    { T{} };
    { ::new (static_cast<void*>(nullptr)) T };
};

template <typename T>
concept signed_integral = std::is_integral_v<T> && std::is_signed_v<T>;
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
concept Deref = requires(T t) { *t; };

template <typename D>
struct DerefTargetImpl;

template <Deref D>
struct DerefTargetImpl<D> {
    using T = decltype(*std::declval<D>());
};

template <typename D>
    requires(!Deref<D>)
struct DerefTargetImpl<D> {
    using T = std::nullptr_t;
};

template <typename D>
using DerefTarget = typename DerefTargetImpl<D>::T;

enum class State : uint8_t { Ok, Err, MovedFrom };

} // namespace detail
#endif

template <typename T, typename E>
class Result;

/// An error, held by Result.
template <typename E>
class Err {
private:
    using Inner = detail::InnerType<E>;

    Inner m_err;

    template <typename F>
    friend class Err;

    template <typename T, typename F>
    friend class Result;

public:
    /// Construct an Err from something convertible to its inner type.
    template <typename F>
    // NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
        requires(detail::constructible_from<Inner, F>)
    explicit Err(F&& err) : m_err{std::forward<F>(err)} {}

    /// Convert an Err from one inner type to another.
    template <typename F>
        requires(detail::constructible_from<Inner, typename Err<F>::Inner>)
    explicit Err(Err<F>&& err) : m_err{std::move(err.m_err)} {}

#ifndef DOXYGEN
    template <typename F>
        requires(detail::constructible_from<Inner, typename Err<F>::Inner>)
    explicit Err(Err<F> const& err) : m_err{err.m_err} {}

    template <typename F>
        requires(detail::IS_REF<E> &&
                 detail::constructible_from<Inner, typename Err<F>::Inner const&>)
    explicit Err(Err<F> const& err) : m_err{err.m_err} {}

    template <typename F>
        requires(detail::IS_REF<E> && detail::constructible_from<Inner, typename Err<F>::Inner&>)
    explicit Err(Err<F>& err) : m_err{err.m_err} {}
#endif

    /// Get the contents of the Err.
    E err() const& { return m_err; }

    /// @copydoc err
    E err() && { return std::move(m_err); }
};

#ifndef DOXYGEN
template <typename E>
Err(E&&) -> Err<E>;
#endif

template <typename T, typename E>
class [[nodiscard]] Result {
private:
    using Inner = detail::InnerType<T>;
    using State = detail::State;

    template <typename U, typename F>
    friend class Result;

    union {
        Inner  m_ok;
        Err<E> m_err;
    };
    State m_state;

public:
    Result()
        requires detail::default_initializable<Inner>
        : m_ok{}, m_state{State::Ok} {}

    /// Construct a Result from something convertible to the successful variant.
    template <typename U = T>
        requires(detail::constructible_from<Inner, U> && !std::same_as<Err<E>, U> &&
                 // NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
                 !std::same_as<Result<T, E>, U>)
    Result(U&& value) : m_ok{std::forward<U>(value)}, m_state{State::Ok} {}

    /// Construct a Result from an Err.
    template <typename F = E>
        requires(detail::constructible_from<E, F>)
    Result(Err<F>&& err) : m_err{std::forward<Err<F>>(err)}, m_state{State::Err} {}

#ifndef DOXYGEN
    template <typename U, typename F>
        requires(detail::constructible_from<Inner, typename Result<U, F>::Inner const&> &&
                 detail::constructible_from<E, F const&> && !std::same_as<T, U> &&
                 !std::same_as<E, F>)
    Result(Result<U, F> const& other) : m_state{other.m_state} {
        switch (m_state) {
        case State::Ok:
            new (&m_ok) Inner{other.m_ok};
            break;
        case State::Err:
            new (&m_err) Err<E>{other.m_err};
            break;
        case State::MovedFrom:
            break;
        }
    }

    template <typename U, typename F>
        requires(detail::constructible_from<Inner, typename Result<U, F>::Inner&> &&
                 detail::constructible_from<E, F&> && !std::same_as<T, U> && !std::same_as<E, F>)
    Result(Result<U, F>& other) : m_state{other.m_state} {
        switch (m_state) {
        case State::Ok:
            new (&m_ok) Inner{other.m_ok};
            break;
        case State::Err:
            new (&m_err) Err<E>{other.m_err};
            break;
        case State::MovedFrom:
            break;
        }
    }
#endif

    /// Convert a Result between compatible types.
    template <typename U, typename F>
        requires(detail::constructible_from<Inner, typename Result<U, F>::Inner> &&
                 detail::constructible_from<E, F> && !std::same_as<T, U> && !std::same_as<E, F>)
    Result(Result<U, F>&& other) : m_state{other.m_state} {
        other.m_state = State::MovedFrom;
        switch (m_state) {
        case State::Ok:
            new (&m_ok) Inner{other.m_ok};
            break;
        case State::Err:
            new (&m_err) Err<E>{other.m_err};
            break;
        case State::MovedFrom:
            break;
        }
    }

#ifndef DOXYGEN
    // TODO: Add variants of copy and move constructors and assignment operators for trivial types.
    Result(Result const& other) : m_state{other.m_state} {
        switch (m_state) {
        case State::Ok:
            new (&m_ok) Inner{other.m_ok};
            break;
        case State::Err:
            new (&m_err) Err{other.m_err};
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
            new (&m_ok) Inner{other.m_ok};
            break;
        case State::Err:
            new (&m_err) Err{other.m_err};
            break;
        case State::MovedFrom:
            break;
        }

        return *this;
    }

    Result(Result&& other) noexcept : m_state{other.m_state} {
        other.m_state = State::MovedFrom;
        switch (m_state) {
        case State::Ok:
            new (&m_ok) Inner{std::move(other.m_ok)};
            break;
        case State::Err:
            new (&m_err) Err{std::move(other.m_err)};
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
            new (&m_ok) Inner{std::move(other.m_ok)};
            break;
        case State::Err:
            new (&m_err) Err{std::move(other.m_err)};
            break;
        case State::MovedFrom:
            break;
        }

        return *this;
    }

    // TODO: Turn these into Clang version checks once it supports prospective destructors.
#ifndef __clang__
    ~Result() = default;
#endif

    ~Result()
#ifndef __clang__
        requires(!std::is_trivially_destructible_v<Inner> ||
                 !std::is_trivially_destructible_v<Err<E>>)
#endif
    {
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
#endif

    /// Check whether the Result has a value.
    bool is_ok() const { return m_state == State::Ok; }
    /// Check whether the Result is an error.
    bool is_err() const { return m_state == State::Err; }

    /// Return the contents of the Result if it is not an error, or panic.
    T unwrap() && {
        A3_UNWRAPND(is_ok());
        m_state = State::MovedFrom;
        return std::move(m_ok);
    }

#ifndef DOXYGEN
    T unwrap() const&
        requires(std::is_trivially_copyable_v<Inner>)
    {
        A3_UNWRAPND(is_ok());
        return m_ok;
    }
#endif

    /// Return the error contained by the Result, if present, or panic.
    E unwrap_err() && {
        A3_UNWRAPND(!is_ok());
        m_state = State::MovedFrom;
        return std::move(m_err).err();
    }

#ifndef DOXYGEN
    E unwrap_err() const&
        requires(std::is_trivially_copyable_v<Err<E>>)
    {
        A3_UNWRAPND(!is_ok());
        return m_err.err();
    }
#endif

    /// Return the value contained by the Result, or the given fallback.
    T unwrap_or(T fallback) && {
        if (is_ok()) {
            m_state = State::MovedFrom;
            return std::move(m_ok);
        }
        return std::move(fallback);
    }

#ifndef DOXYGEN
    T unwrap_or(T fallback) const&
        requires(std::is_trivially_copyable_v<Inner>)
    {
        if (is_ok())
            return m_ok;
        return fallback;
    }
#endif

    /// Return the value contained by the Result, or call the given fallback function.
    template <detail::invocable<E> Fn>
        T unwrap_or_else(Fn&& f) &&
        requires(detail::constructible_from<T, std::invoke_result_t<Fn, E>>) {
            auto state = std::exchange(m_state, State::MovedFrom);
            if (state == State::Ok) {
                return std::move(m_ok);
            }
            return std::forward<Fn>(f)(std::move(m_err));
        }

#ifndef DOXYGEN
        template <detail::invocable<E> Fn>
        T unwrap_or_else(Fn&& f) const&
            requires(detail::constructible_from<T, std::invoke_result_t<Fn, E>> &&
                     std::is_trivially_copyable_v<Inner>)
    {
        if (is_ok())
            return m_ok;
        return std::forward<Fn>(f)(m_err);
    }
#endif

    /// \brief Transform the contents of a Result.
    ///
    /// Apply a lambda to the contents (if any) of the Result, returning the result. This is much
    /// like `std::optional`'s upcoming `transform` API. See also Option::map.
    template <detail::invocable<T> Fn>
    Result<std::invoke_result_t<Fn, T>, E> map(Fn&& f) &&
        requires(!detail::IS_REF<T>)
    {
        auto state = std::exchange(m_state, State::MovedFrom);
        switch (state) {
        case State::Ok:
            return std::forward<Fn>(f)(std::move(m_ok));
        case State::Err:
            return std::move(m_err);
        case State::MovedFrom:
            A3_PANIC("Result::map on moved-from Result.");
        }

        A3_UNREACHABLE();
    }

#ifndef DOXYGEN
    template <detail::invocable<T> Fn>
    Result<std::invoke_result_t<Fn, T>, E> map(Fn&& f) &&
        requires(detail::IS_REF<T>)
    {
        auto state = std::exchange(m_state, State::MovedFrom);
        switch (state) {
        case State::Ok:
            return std::forward<Fn>(f)(m_ok.get());
        case State::Err:
            return std::move(m_err);
        case State::MovedFrom:
            A3_PANIC("Result::map on moved-from Result.");
        }

        A3_UNREACHABLE();
    }
#endif

    /// Like Result::map, but returns a fallback if it is an error.
    template <detail::invocable<T> Fn, typename U = std::invoke_result_t<Fn, T>>
    U map_or(U&& fallback, Fn&& f)
        requires(detail::constructible_from<U, std::invoke_result_t<Fn, T>>)
    {
        if (is_err())
            return std::forward<U>(fallback);
        return std::move(*this).map(std::forward<Fn>(f)).m_ok;
    }

    /// Like Result::map_or, but calls a function for fallback.
    template <
        detail::invocable<T> Fn, detail::invocable<E> FFn,
        typename U = std::common_type_t<std::invoke_result_t<Fn, T>, std::invoke_result_t<FFn, E>>>
    U map_or_else(FFn&& ff, Fn&& f)
        requires(detail::constructible_from<U, std::invoke_result_t<Fn, T>> &&
                 detail::constructible_from<U, std::invoke_result_t<FFn, E>>)
    {
        if (is_err()) {
            m_state = State::MovedFrom;
            return std::forward<FFn>(ff)(std::move(m_err).err());
        }
        return std::move(*this).map(std::forward<Fn>(f)).m_ok;
    }

    /// Like Result::map, but over the error variant.
    template <detail::invocable<E> Fn>
    Result<T, std::invoke_result_t<Fn, E>> map_err(Fn&& f) &&
        requires(!detail::IS_REF<E>)
    {
        auto state = std::exchange(m_state, State::MovedFrom);
        switch (state) {
        case State::Ok:
            return std::move(m_ok);
        case State::Err:
            return Err{std::forward<Fn>(f)(std::move(m_err).err())};
        case State::MovedFrom:
            A3_PANIC("Result::map_err on moved-from Result.");
        }

        A3_UNREACHABLE();
    }

#ifndef DOXYGEN
    template <detail::invocable<E> Fn>
    Result<T, std::invoke_result_t<Fn, E>> map_err(Fn&& f) &&
        requires(detail::IS_REF<E>)
    {
        auto state = std::exchange(m_state, State::MovedFrom);
        switch (state) {
        case State::Ok:
            return std::move(m_ok);
        case State::Err:
            return Err{std::forward<Fn>(f)(m_err.err())};
        case State::MovedFrom:
            A3_PANIC("Result::map_err on moved-from Result.");
        }

        A3_UNREACHABLE();
    }
#endif

    /// Convert a reference to a Result to a Result of references.
    Result<T&, E&> as_ref() & { return *this; }
    /// @copydoc as_ref
    Result<T const&, E const&> as_ref() const& { return *this; }

    /// Dereference the value, if any, and return a Result containing a reference.
    Result<detail::DerefTarget<T>&, E&> as_deref() &
        requires detail::Deref<T&>
    {
        return as_ref().map([](auto& v) -> auto& { return *v; });
    }

#ifndef DOXYGEN
    Result<detail::DerefTarget<T> const&, E const&> as_deref() const&
        requires detail::Deref<T const&>
    {
        return as_ref().map([](auto const& v) -> auto const& { return *v; });
    }
#endif

    /// Return the error variant, if present.
    std::optional<typename Err<E>::Inner> err() && {
        if (!is_err())
            return std::nullopt;
        m_state = State::MovedFrom;
        return std::move(m_err).err();
    }

    /// Return the success variant, if present.
    std::optional<Inner> ok() && {
        if (!is_ok())
            return std::nullopt;
        m_state = State::MovedFrom;
        return std::move(m_ok);
    }
};

#if defined(__GNUC__) || defined(__clang__) || defined(DOXYGEN)
/// \brief An imitation of Rust's ? operator (and formerly try macro)
///
/// Evaluate an expression which returns a Result. If it is successful, simply return the contents.
/// If not, return early from the enclosing function with the error variant. The enclosing function
/// must return Result.
#define A3_RTRY(R)                                                                                 \
    ({                                                                                             \
        auto _tmp = (R);                                                                           \
        if (_tmp.is_err())                                                                         \
            return a3::Err{std::move(_tmp).unwrap_err()};                                          \
        if (!_tmp.is_ok())                                                                         \
            A3_PANIC("A3_RTRY on moved-from Result.");                                             \
        std::move(_tmp).unwrap();                                                                  \
    })
#endif

/// Construct a Result from a C-style error code.
inline Err<std::error_code> error_code(int code) {
    return Err{std::error_code{code, std::system_category()}};
}

/// \brief Construct a Result from a signed return.
///
/// Construct a Result from a value where nonnegative values are successful, and negative values
/// encode an error.
template <detail::signed_integral R>
Result<std::make_unsigned_t<R>, std::error_code> signed_result(R val) {
    if (val < R{0})
        return error_code(static_cast<int>(-val));
    return static_cast<std::make_unsigned_t<R>>(val);
}

/// \brief Construct a Result from a signed return with `errno`.
///
/// Construct a Result from a value where nonnegative values are successful, and errors are stored
/// in `errno`.
template <detail::signed_integral R>
Result<std::make_unsigned_t<R>, std::error_code> errno_result(R val) {
    if (val < R{0})
        return error_code(errno);
    return static_cast<std::make_unsigned_t<R>>(val);
}

} // namespace a3

#endif
