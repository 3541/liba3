/*
 * VARARG FORWARDING
 *
 * Copyright (c) 2024, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 *
 * No compiler is able to inline functions which use va_start.
 * - GCC errors if always_inline is marked on such a function, but provides a builtin to permit
 *   inlining while forwarding varargs.
 * - Clang does not provide the builtin, but also does not error (silently failing to inline).
 * - MSVC is as usualthe worst of both worlds, producing an error and not providing an alternative.
 */

#pragma once

#include <a3/shim/format.h>
#include <stdarg.h>

#include <a3/types.h>

#ifdef __has_builtin
#if __has_builtin(__builtin_va_arg_pack)
#define A3_USE_VARARG_PACK
#endif
#elif defined(__GNUC__)
#define A3_USE_VARARG_PACK
#endif

#ifdef A3_USE_VARARG_PACK
#define A3_VA_FWD_ARG(NAME) ...
#define A3_VA_FWD_DECL(NAME, PREV)
#define A3_VA_FWD_PASS(NAME) __builtin_va_arg_pack()

#define A3_VA_FWD_START(NAME, PREV)                                                                \
    va_list NAME;                                                                                  \
    va_start(NAME, PREV)

#define A3_VA_FWD_FMT(FMT_INDEX, VARG_INDEX) A3_FORMAT_FN(FMT_INDEX, VARG_INDEX)

#define A3_VA_FWD_INLINE A3_ALWAYS_INLINE

#else
#define A3_VA_FWD_ARG(...) va_list __VA_ARGS__
#define A3_VA_FWD_DECL(NAME, PREV)                                                                 \
    va_list NAME;                                                                                  \
    va_start(NAME, PREV)
#define A3_VA_FWD_PASS(NAME) NAME

#define A3_VA_FWD_START(NAME, PREV)

#define A3_VA_FWD_FMT(FMT_INDEX, VARG_INDEX) A3_FORMAT_FN(FMT_INDEX, 0)

#ifdef _MSC_VER
#define A3_VA_FWD_INLINE static inline // :(
#else
#define A3_VA_FWD_INLINE A3_ALWAYS_INLINE
#endif

#endif

#undef A3_USE_VARARG_PACK
