/*
 * UTIL -- Miscellaneous utility macros and definitions.
 *
 * Copyright (c) 2020-2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <a3/log.h>
#include <a3/macro.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// Abort with a message.
#define A3_PANIC_FMT(fmt, ...)                                                                     \
    do {                                                                                           \
        A3_ERR_FMT(fmt, __VA_ARGS__);                                                              \
        assert(false);                                                                             \
        exit(EXIT_FAILURE);                                                                        \
    } while (0)

#define A3_PANIC(msg) A3_PANIC_FMT("%s", (msg))

#define A3_UNREACHABLE() A3_PANIC("UNREACHABLE")

#ifdef _MSC_VER
// Disable warning for "assignment within conditional expression. Why is this
// even a warning?
#pragma warning(disable : 4706)
#endif

#if defined(__GNUC__) || defined(__clang__)
#define A3_LIKELY(E)   (__builtin_expect(!!(E), 1))
#define A3_UNLIKELY(E) (__builtin_expect(!!(E), 0))
#elif defined(__cplusplus) && __cplusplus >= 202002L
#define A3_LIKELY(E)   (E) [[likely]]
#define A3_UNLIKELY(E) (E) [[unlikely]]
#else
#define A3_LIKELY(E)   (E)
#define A3_UNLIKELY(E) (E)
#endif

// "unwrap" a return value which is falsy on error, and assign to T on success.
// This is useful for fatal errors (e.g., allocation failures).
#define A3_UNWRAPN(T, X)                                                                           \
    do {                                                                                           \
        if A3_UNLIKELY (!(T = X)) {                                                                \
            A3_PANIC_FMT("UNWRAP(%s)", #X);                                                        \
        }                                                                                          \
    } while (0)

// Unwrap a return value and assign it to a new variable of the given type.
#define A3_UNWRAPNI(TY, T, X)                                                                      \
    TY T;                                                                                          \
    A3_UNWRAPN(T, X)

// Unwrap a return value which is negative on error and ignore the result
// otherwise (i.e., unwrap-sign-discard).
#define A3_UNWRAPSD(X)                                                                             \
    do {                                                                                           \
        if A3_UNLIKELY ((X) < 0) {                                                                 \
            A3_PANIC_FMT("UNWRAP(%s)", #X);                                                        \
        }                                                                                          \
    } while (0)

#define A3_UNWRAPND(X)                                                                             \
    do {                                                                                           \
        if A3_UNLIKELY (!(X)) {                                                                    \
            A3_PANIC_FMT("UNWRAP(%s)", #X);                                                        \
        }                                                                                          \
    } while (0)

// Unwrap a signed return value and keep the result.
#define A3_UNWRAPS(T, X)                                                                           \
    do {                                                                                           \
        if A3_UNLIKELY ((T = X) < 0) {                                                             \
            A3_PANIC_FMT("UNWRAP(%s)", #X);                                                        \
        }                                                                                          \
    } while (0)

/// Bubble up an error condition, mapping the error to the given value.
#define A3_TRYB_MAP(T, E)                                                                          \
    do {                                                                                           \
        if A3_UNLIKELY (!(T))                                                                      \
            return E;                                                                              \
    } while (0)

/// Similar to ::A3_TRYB_MAP, but with a custom condition. For example, `A3_TRY_COND(x, < 0, false)`
/// will return false if `x < 0`.
#define A3_TRY_COND(T, C, E)                                                                       \
    do {                                                                                           \
        if A3_UNLIKELY ((T)C) {                                                                    \
            return E;                                                                              \
        }                                                                                          \
    } while (0)

/// Bubble up an error condition. Requires the caller to return a boolean and the callee to return a
/// falsy value on failure.
#define A3_TRYB(T) A3_TRYB_MAP(T, false)

// Bubble up an error condition, printing a message on failure at the specified log level.
#define A3_TRYB_MSG(T, L, M)                                                                       \
    do {                                                                                           \
        if A3_UNLIKELY (!(T)) {                                                                    \
            a3_log_msg((L), (M));                                                                  \
            return false;                                                                          \
        }                                                                                          \
    } while (0)

// Map a truthy/falsy return to something else.
#define A3_RET_MAP(F, T, E)                                                                        \
    do {                                                                                           \
        return F ? T : E;                                                                          \
    } while (0)

/// A nicer interface to ::A3_TRY_COND, ::A3_TRYB_MAP, and ::A3_TRYB. When invoked with a single
/// argument, it is equivalent to ::A3_TRYB. Two arguments invokes ::A3_TRYB_MAP, and three
/// arguments invokes ::A3_TRY_COND. Note that the argument order of ::A3_TRY_COND is not consistent
/// with ::A3_TRYB_MAP — the condition comes before the return value.
#define A3_TRY(...)                                                                                \
    A3_M_MSVC_VA_HACK(A3_M_ARG4(__VA_ARGS__, A3_TRY_COND, A3_TRYB_MAP, A3_TRYB, )(__VA_ARGS__))

#if defined(__GNUC__) || defined(__clang__)
#define A3_FORMAT_FN(FMT_INDEX, VARG_INDEX)                                                        \
    __attribute__((__format__(__printf__, FMT_INDEX, VARG_INDEX)))
#else
#define A3_FORMAT_FN(FMT_INDEX, VARG_INDEX)
#endif

#define A3_CONTAINER_OF(PTR, TY, FIELD) ((TY*)((uintptr_t)(void*)PTR - offsetof(TY, FIELD)))
