/*
 * UTIL -- Miscellaneous utility macros and definitions.
 *
 * Copyright (c) 2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#define A3_NO_COPY(T)                                                                              \
public:                                                                                            \
    T(T const&) = delete;                                                                          \
    T& operator=(T const&) = delete

#define A3_NO_MOVE(T)                                                                              \
public:                                                                                            \
    T(T&&)     = delete;                                                                           \
    T& operator=(T&&) = delete

#define A3_PINNED(T)                                                                               \
    A3_NO_COPY(T);                                                                                 \
    A3_NO_MOVE(T)

#define A3_DEFAULT_COPY(T)                                                                         \
    T(T const&) = default;                                                                         \
    T& operator=(T const&) = default

#define A3_DEFAULT_MOVE(T)                                                                         \
    T(T&&)     = default;                                                                          \
    T& operator=(T&&) = default

#define A3_DEFAULTS(T)                                                                             \
    A3_DEFAULT_COPY(T);                                                                            \
    A3_DEFAULT_MOVE(T)
