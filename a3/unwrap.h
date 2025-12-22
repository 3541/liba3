/*
 * UNWRAP — Conditional panic macros.
 *
 * Copyright (c) 2020-2022, 2025, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include "a3/log.h"
#include "a3/shim/likely.h"

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
