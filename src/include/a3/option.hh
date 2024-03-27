/*
 * OPTION -- Nicer std::optional.
 *
 * Copyright (c) 2022, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

/// \file option.hh
/// # Option
///  An extension of std::optional, primarily to add conversions to a3::Result, specialization for
/// reference types, and map-type functions.

#pragma once

#if __cplusplus < 201703L && !defined(DOXYGEN)
#warning "C++17 or greater is required for std::optional."
#else

#include <functional>
#include <optional>

#ifdef __cpp_concepts
#include <a3/result.hh>
#endif

namespace a3 {

template <typename T>
class Option;

namespace detail {

template <typename T>
class OptionBase : public std::optional<T> {
public:
    using std::optional<T>::optional;

    /// Convert a reference to an Option to an optional reference.
    Option<T&> as_ref() & {
        if (!this->has_value())
            return std::nullopt;
        return std::ref(this->value());
    }

    /// @copydoc as_ref
    Option<T const&> as_ref() const& {
        if (!this->has_value())
            return std::nullopt;
        return std::cref(this->value());
    }
};

#ifndef DOXYGEN
template <typename T>
class OptionBase<T&> : public std::optional<std::reference_wrapper<T>> {
public:
    using std::optional<std::reference_wrapper<T>>::optional;

    T& value() { return std::optional<std::reference_wrapper<T>>::value().get(); }
};
#endif

} // namespace detail

/// A wrapper for `std::optional` with some nice add-ons.
template <typename T>
class Option : public detail::OptionBase<T> {
public:
    using detail::OptionBase<T>::OptionBase;

    /// Construct (or implicitly convert) an Option from a `std::optional`.
    template <typename U>
    Option(std::optional<U>&& o) : detail::OptionBase<T> { std::forward<std::optional<U>>(o) } {}

#if !defined(__cpp_lib_optional) || __cpp_lib_optional < 202110L
    /// \brief Transform the contents of an Option.
    ///
    /// Apply a lambda to the contents (if any) of the Option, returning the result. This is much
    /// like `std::optional`'s upcoming `transform` API.
    template <typename Fn, typename U = std::invoke_result_t<Fn, T>>
    Option<U> map(Fn&& f) && {
        if (!this->has_value())
            return {};
        return std::forward<Fn>(f)(std::move(*this).value());
    }
#else
    template <typename Fn, typename U = std::invoke_result_t<Fn, T>>
    Option<U> map(Fn&& f) && {
        return std::move(*this).transform(std::forward<Fn>(f));
    }
#endif

    /// \brief Like Option::map, but return a non-optional type.
    ///
    /// Calls the provided fallback function if `this` is empty.
    template <typename Fn, typename FFn>
    std::invoke_result_t<Fn, T> map_or_else(FFn&& fallback, Fn&& f) && {
        if (!this->has_value())
            return std::forward<FFn>(fallback)();
        return std::move(*this).map(std::forward<Fn>(f)).value();
    }

    /// Like Option::map_or_else, but the fallback is simply a value, rather than a lambda.
    template <typename Fn, typename U = std::invoke_result_t<Fn, T>>
    std::invoke_result_t<Fn, T> map_or(U&& fallback, Fn&& f) && {
        return std::move(*this).map_or_else([&fallback] { return std::forward<U>(fallback); },
                                            std::forward<Fn>(f));
    }

#if defined(__cpp_concepts) || defined(DOXYGEN)
    /// \brief Map `this` onto a Result.
    ///
    /// Constructs the error variant using the given lambda, if required.
    template <typename EFn>
    Result<T, std::invoke_result_t<EFn>> ok_or_else(EFn&& ef) && {
        if (!this->has_value())
            return Err { std::forward<EFn>(ef)() };
        return std::move(*this).value();
    }

    /// Like Option::ok_or_else, but the error variant is given as a simple value.
    template <typename E>
    Result<T, E> ok_or(E&& err) && {
        return std::move(*this).ok_or_else([&err] { return std::forward<E>(err); });
    }
#endif
};

} // namespace a3

#endif
