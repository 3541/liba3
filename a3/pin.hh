/*
 * PIN  — Convenience macros for uncopyable/immovable types.
 *
 * Copyright (c) 2022, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

#pragma once

/// Make a type non-copyable.
#define A3_NO_COPY(T)                                                                              \
public:                                                                                            \
    T(T const&)            = delete;                                                               \
    T& operator=(T const&) = delete

/// Make a type non-moveable.
#define A3_NO_MOVE(T)                                                                              \
public:                                                                                            \
    T(T&&)            = delete;                                                                    \
    T& operator=(T&&) = delete

/// Make a type both non-copyable and non-moveable.
#define A3_PINNED(T)                                                                               \
    A3_NO_COPY(T);                                                                                 \
    A3_NO_MOVE(T)
