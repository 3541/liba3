/*
 * UTIL -- Miscellaneous utility macros and definitions.
 *
 * Copyright (c) 2020-2021, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <a3/log.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// Abort with a message.
#define A3_PANIC_FMT(fmt, ...)                                                                     \
    do {                                                                                           \
        A3_ERR_FMT(fmt, __VA_ARGS__);                                                              \
        exit(EXIT_FAILURE);                                                                        \
    } while (0)

#define A3_PANIC(msg) A3_PANIC_FMT("%s", (msg))

#define A3_UNREACHABLE() A3_PANIC("UNREACHABLE")

#ifdef _MSC_VER
// Disable warning for "assignment within conditional expression. Why is this
// even a warning?
#pragma warning(disable : 4706)
#endif

// "unwrap" a return value which is falsy on error, and assign to T on success.
// This is useful for fatal errors (e.g., allocation failures).
#define A3_UNWRAPN(T, X)                                                                           \
    do {                                                                                           \
        if (!(T = X)) {                                                                            \
            A3_PANIC_FMT("UNWRAP(%s)", #X);                                                        \
        }                                                                                          \
    } while (0)

// Unwrap a return value which is negative on error and ignore the result
// otherwise (i.e., unwrap-sign-discard).
#define A3_UNWRAPSD(X)                                                                             \
    do {                                                                                           \
        if ((X) < 0) {                                                                             \
            A3_PANIC_FMT("UNWRAP(%s)", #X);                                                        \
        }                                                                                          \
    } while (0)

#define A3_UNWRAPND(X)                                                                             \
    do {                                                                                           \
        if (!(X)) {                                                                                \
            A3_PANIC_FMT("UNWRAP(%s)", #X);                                                        \
        }                                                                                          \
    } while (0)

// Unwrap a signed return value and keep the result.
#define A3_UNWRAPS(T, X)                                                                           \
    do {                                                                                           \
        if ((T = X) < 0) {                                                                         \
            A3_PANIC_FMT("UNWRAP(%s)", #X);                                                        \
        }                                                                                          \
    } while (0)

// Bubble up an error condition. Requires the caller to return a boolean and the
// callee to return a falsy value on failure.
#define A3_TRYB(T)                                                                                 \
    do {                                                                                           \
        if (!(T))                                                                                  \
            return false;                                                                          \
    } while (0);

// Bubble up an error condition, mapping the error to the given value.
#define A3_TRYB_MAP(T, E)                                                                          \
    do {                                                                                           \
        if (!(T))                                                                                  \
            return E;                                                                              \
    } while (0);

// Map a truthy/falsy return to something else.
#define A3_RET_MAP(F, T, E)                                                                        \
    do {                                                                                           \
        return F ? T : E;                                                                          \
    } while (0);

#if defined(__GNUC__) || defined(__clang__)
#define A3_FORMAT_FN(FMT_INDEX, VARG_INDEX)                                                        \
    __attribute__((__format__(__printf__, FMT_INDEX, VARG_INDEX)))
#else
#define A3_FORMAT_FN(FMT_INDEX, VARG_INDEX)
#endif

#define A3_CONTAINER_OF(PTR, TY, FIELD) ((TY*)((uintptr_t)PTR - offsetof(TY, FIELD)))
