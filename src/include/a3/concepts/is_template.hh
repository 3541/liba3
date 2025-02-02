/*
 * IsTemplate -- Check whether a type is an instance of a particular template.
 *
 * Copyright (c) 2024, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

namespace a3 {

namespace detail::is_template {

template <template <typename...> typename Tp, typename T>
constexpr bool IS_TEMPLATE{false};

template <template <typename...> typename Tp, typename... Args>
constexpr bool IS_TEMPLATE<Tp, Tp<Args...>>{true};

} // namespace detail::is_template

template <typename T, template <typename...> typename Tp>
concept IsTemplate = detail::is_template::IS_TEMPLATE<Tp, T>;

namespace test {

template <typename>
struct T {};

static_assert(IsTemplate<T<int>, T>);

} // namespace test

} // namespace a3
