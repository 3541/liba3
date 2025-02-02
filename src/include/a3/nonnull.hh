/*
 * NONNULL -- A nonnull pointer.
 *
 * Copyright (c) 2023, Alex O'Brien <3541@3541.website>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of
 * the MPL was not distributed with this file, you can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#ifdef __cpp_lib_concepts

#include <concepts>
#include <cstddef>
#include <memory>
#include <type_traits>

#include <a3/concepts/is_template.hh>
#include <a3/fwd.hh>
#include <a3/util.h>

namespace a3 {

namespace detail::nonnull {

template <typename T>
concept Indexable = std::is_array_v<T> || requires(T value, std::size_t i) {
    { value[i] } -> std::same_as<typename T::element_type&>;
};

template <typename T>
concept Deref = requires(T value) {
    { *value };
};

template <typename T>
struct Traits : public std::pointer_traits<T> {
    using Value = T;
};

template <typename T>
struct Traits<T[]> {
    using pointer      = T*;
    using element_type = T;

    using Value = pointer;
};

} // namespace detail::nonnull

template <typename T>
concept Pointer = requires(T t) {
    typename detail::nonnull::Traits<T>::pointer;
    { static_cast<bool>(t) };
};

template <Pointer T>
struct NonNull;

template <typename T>
concept IsNonNull = IsTemplate<T, NonNull>;

template <Pointer T>
struct NonNull {
private:
    using Traits = detail::nonnull::Traits<T>;

    Traits::Value m_ptr;

public:
    using Inner        = T;
    using element_type = Traits::element_type;
    using value_type   = std::conditional_t<detail::nonnull::Indexable<T>, element_type, void>;

    NonNull()               = delete;
    NonNull(std::nullptr_t) = delete;

    constexpr explicit NonNull(T) noexcept
        requires(!IsNonNull<T>);

    template <typename U>
        requires std::convertible_to<U, T>
    constexpr NonNull(NonNull<U>) noexcept(std::is_nothrow_convertible_v<U, T>);

    constexpr NonNull(NonNull&&) noexcept            = default;
    constexpr NonNull& operator=(NonNull&&) noexcept = default;

    constexpr NonNull(NonNull const&) noexcept            = default;
    constexpr NonNull& operator=(NonNull const&) noexcept = default;

    constexpr element_type&       operator*() noexcept;
    constexpr element_type const& operator*() const noexcept;

    constexpr element_type*       operator->() noexcept;
    constexpr element_type const* operator->() const noexcept;

    constexpr element_type& operator[](std::size_t)
        requires detail::nonnull::Indexable<T>;
    constexpr element_type const& operator[](std::size_t) const
        requires detail::nonnull::Indexable<T>;

    constexpr auto operator<=>(NonNull const&) const noexcept = default;

    constexpr operator NonNull<element_type const*>() const noexcept
        requires std::is_pointer_v<T>;

    constexpr std::decay_t<T> get() && noexcept(std::is_nothrow_move_constructible_v<T>);
};

template <Pointer T>
constexpr NonNull<T>::NonNull(T ptr) noexcept
    requires(!IsNonNull<T>)
    : m_ptr{std::move(ptr)} {
    if (!m_ptr)
        A3_PANIC("Attempted to construct a NonNull from a null pointer.");
}

template <Pointer T>
template <typename U>
    requires std::convertible_to<U, T>
constexpr NonNull<T>::NonNull(NonNull<U> other) noexcept(std::is_nothrow_convertible_v<U, T>) :
    NonNull{static_cast<T>(std::move(other).get())} {}

template <Pointer T>
constexpr auto NonNull<T>::operator*() noexcept -> element_type& {
    if constexpr (detail::nonnull::Deref<T>) {
        return *m_ptr;
    } else {
        static_assert(detail::nonnull::Indexable<T>);
        return m_ptr[0];
    }
}

template <Pointer T>
constexpr auto NonNull<T>::operator*() const noexcept -> element_type const& {
    return **const_cast<NonNull*>(this);
}

template <Pointer T>
constexpr auto NonNull<T>::operator->() noexcept -> element_type* {
    return &**this;
}

template <Pointer T>
constexpr auto NonNull<T>::operator->() const noexcept -> element_type const* {
    return &**this;
}

template <Pointer T>
constexpr auto NonNull<T>::operator[](std::size_t index) -> element_type&
    requires detail::nonnull::Indexable<T>
{
    return m_ptr[index];
}

template <Pointer T>
constexpr auto NonNull<T>::operator[](std::size_t index) const -> element_type const&
    requires detail::nonnull::Indexable<T>
{
    return m_ptr[index];
}

template <Pointer T>
constexpr NonNull<T>::operator NonNull<element_type const*>() const noexcept
    requires std::is_pointer_v<T>
{
    return NonNull<element_type const*>{m_ptr};
}

template <Pointer T>
constexpr std::decay_t<T> NonNull<T>::get() && noexcept(std::is_nothrow_move_constructible_v<T>) {
    return std::move(*this).m_ptr;
}

template <typename T>
using Box = NonNull<std::unique_ptr<T>>;

template <typename T>
auto make_box(auto&&... args) {
    return NonNull{std::make_unique<T>(A3_FWD(args)...)};
}

} // namespace a3

#endif
