/*
 * UTIL -- Miscellaneous utility macros and definitions.
 *
 * Copyright (c) 2022, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

/// \file util.hh

#pragma once

#include <cstdlib>

/// Make a type non-copyable.
#define A3_NO_COPY(T)                                                                              \
public:                                                                                            \
    T(T const&) = delete;                                                                          \
    T& operator=(T const&) = delete

/// Make a type non-moveable.
#define A3_NO_MOVE(T)                                                                              \
public:                                                                                            \
    T(T&&)     = delete;                                                                           \
    T& operator=(T&&) = delete

/// Make a type both non-copyable and non-moveable.
#define A3_PINNED(T)                                                                               \
    A3_NO_COPY(T);                                                                                 \
    A3_NO_MOVE(T)

/// Explicitly request the default copy operations.
#define A3_DEFAULT_COPY(T)                                                                         \
public:                                                                                            \
    T(T const&) = default;                                                                         \
    T& operator=(T const&) = default

/// Explicitly request the default move operations.
#define A3_DEFAULT_MOVE(T)                                                                         \
public:                                                                                            \
    T(T&&) noexcept = default;                                                                     \
    T& operator=(T&&) noexcept = default

/// Explicitly request the default copy and move operations.
#define A3_DEFAULTS(T)                                                                             \
    A3_DEFAULT_COPY(T);                                                                            \
    A3_DEFAULT_MOVE(T)

namespace a3 {

/// A deleter for use with `std::unique_ptr` when storing memory allocated with `malloc`.
struct MallocDeleter {
    /// Invoke `free`.
    void operator()(void* ptr) { free(ptr); }
};

} // namespace a3
