/*
 * OPTION — Nicer std::optional.
 *
 * Copyright (c) 2022, 2024, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

/// \file option.hh
/// # Option
///  An extension of std::optional, providing correct behavior for reference types.

#pragma once

#include <functional>
#include <optional>
#include <type_traits>

#ifdef __cpp_lib_concepts
#include <concepts>
#endif

#include <a3/fwd.hh>

namespace a3 {

template <typename T>
class Option;

namespace detail {

#ifdef __cpp_lib_concepts
template <typename F, typename... Args>
concept Invocable = std::invocable<F, Args...>;
#else
template <typename F, typename... Args>
concept Invocable = std::is_invocable_v<F, Args...>;
#endif

template <typename T>
constexpr inline bool IsOptionalImpl = false;

template <typename T>
constexpr inline bool IsOptionalImpl<std::optional<T>> = true;

template <typename T>
constexpr inline bool IsOptional = IsOptionalImpl<std::remove_reference_t<std::remove_cv_t<T>>>;

template <typename T>
class OptionBase : public std::optional<T> {
public:
    using std::optional<T>::optional;

    /// Convert a reference to an Option to an optional reference.
    Option<T&> as_ref() {
        if (!this->has_value())
            return {};

        return **this;
    }

    /// @copydoc as_ref
    Option<T const&> as_ref() const {
        if (!this->has_value())
            return {};

        return **this;
    }
};

#ifndef DOXYGEN
template <typename T>
class OptionBase<T&> : public std::optional<std::reference_wrapper<T>> {
public:
    using std::optional<std::reference_wrapper<T>>::optional;

    T&       value() { return std::optional<std::reference_wrapper<T>>::value(); }
    T const& value() const { return std::optional<std::reference_wrapper<T>>::value(); }

    T& operator*() noexcept {
        return *static_cast<std::optional<std::reference_wrapper<T>>&>(*this);
    }

    T const& operator*() const noexcept {
        return *static_cast<std::optional<std::reference_wrapper<T>> const&>(*this);
    }

    T*       operator->() noexcept { return &**this; }
    T const* operator->() const noexcept { return &**this; }
};
#endif

} // namespace detail

/// A wrapper for `std::optional` with some nice add-ons.
template <typename T>
class Option : public detail::OptionBase<T> {
public:
    using detail::OptionBase<T>::OptionBase;

    /// \brief Transform the contents of an Option.
    ///
    /// Apply a lambda to the contents (if any) of the Option, returning the result. This is much
    /// like `std::optional`'s `transform` API, but plays nice with references.
    template <detail::Invocable<T> Fn, typename U = std::invoke_result_t<Fn, T>>
    Option<U> map(Fn&& f) && {
        if (!this->has_value())
            return {};

        return std::invoke(A3_FWD(f), *std::move(*this));
    }

    /// @copydoc map
    template <detail::Invocable<T> Fn, typename U = std::invoke_result_t<Fn, T>>
        requires std::is_copy_constructible_v<T>
    Option<U> map(Fn&& f) const& {
        return Option<T>{*this}.map(A3_FWD(f));
    }

    /// \brief Like Option::map, but return a non-optional type.
    ///
    /// Calls the provided fallback function if `this` is empty.
    template <detail::Invocable<T> Fn, detail::Invocable FFn>
    std::invoke_result_t<Fn, T> map_or_else(FFn&& fallback, Fn&& f) && {
        if (!this->has_value())
            return std::invoke(A3_FWD(fallback));

        return *std::move(*this).map(A3_FWD(f));
    }

    /// @copydoc map_or_else
    template <detail::Invocable<T> Fn, detail::Invocable FFn>
        requires std::is_copy_constructible_v<T>
    std::invoke_result_t<Fn, T> map_or_else(FFn&& fallback, Fn&& f) const& {
        return Option<T>{*this}.map_or_else(A3_FWD(fallback), A3_FWD(f));
    }

    /// Like Option::map_or_else, but the fallback is simply a value, rather than a lambda.
    template <detail::Invocable<T> Fn, typename U = std::invoke_result_t<Fn, T>>
    std::invoke_result_t<Fn, T> map_or(U&& fallback, Fn&& f) && {
        if (!this->has_value())
            return A3_FWD(fallback);

        return *std::move(*this).map(A3_FWD(f));
    }

    /// @copydoc map_or
    template <detail::Invocable<T> Fn, typename U = std::invoke_result_t<Fn, T>>
        requires std::is_copy_constructible_v<T>
    std::invoke_result_t<Fn, T> map_or(U&& fallback, Fn&& f) const& {
        return Option<T>{*this}.map_or(A3_FWD(fallback), A3_FWD(f));
    }
};

} // namespace a3
