/*
 * TRY — Early-return try macro.
 *
 * Copyright (c) 2020-2022, 2025, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <stdbool.h>

#include "a3/macro.h"
#include "a3/shim/likely.h"

#define A3_TRY_MAP_(C, E)                                                                          \
    A3_M_BEGIN                                                                                     \
        if A3_UNLIKELY (!(C))                                                                      \
            return (E);                                                                            \
    A3_M_END

#define A3_TRY_SIMPLE_(C) A3_TRY_MAP_(C, false)

/// A nicer interface to ::A3_TRY_COND, ::A3_TRYB_MAP, and ::A3_TRYB. When invoked with a single
/// argument, it is equivalent to ::A3_TRYB. Two arguments invokes ::A3_TRYB_MAP, and three
/// arguments invokes ::A3_TRY_COND. Note that the argument order of ::A3_TRY_COND is not consistent
/// with ::A3_TRYB_MAP — the condition comes before the return value.
//
/// Early-return try macro. Supports invocation in two forms:
/// * A3_TRY(expr) early-returns false if expr is falsy.
/// * A3_TRY(expr, value) early-returns value if expr is falsy.
#define A3_TRY(...)                                                                                \
    A3_M_MSVC_VA_HACK(A3_M_ARG3(__VA_ARGS__, A3_TRY_MAP_, A3_TRY_SIMPLE_, )(__VA_ARGS__))
