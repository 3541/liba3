/*
 * LOG -- Simple logging utilities.
 *
 * Copyright (c) 2020-2022, 2024-2025, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 *
 * To set a non-default log level, define A3_LOG_LEVEL before including this file. To avoid
 * confusion, this file should not be included multiple times, or in any header.
 */

#pragma once

#include <stdarg.h>
#include <stdio.h>

#include "a3/shim/cpp.h"
#include "a3/shim/format.h"
#include "a3/shim/likely.h"
#include "a3/shim/export.h"

#ifdef __has_builtin
#if __has_builtin(__builtin_va_arg_pack)
#define A3_USE_VA_PACK
#endif
#elif defined(__GNUC__)
#define A3_USE_VA_PACK
#endif

#ifdef A3_USE_VA_PACK
#define A3_LOG_INLINE A3_ALWAYS_INLINE
#else
#define A3_LOG_INLINE static inline
#endif

A3_H_BEGIN

typedef int A3LogLevel;
#define A3_LOG_TRACE   0
#define A3_LOG_DEBUG   1
#define A3_LOG_INFO    2
#define A3_LOG_WARN    3
#define A3_LOG_ERROR   4
#define A3_LOG_INVALID (~0)

extern FILE*      A3_PRIV_LOG_OUTPUT;
extern A3LogLevel A3_PRIV_LOG_LEVEL;

A3_EXPORT void a3_log_init(FILE*, A3LogLevel);
A3_EXPORT void a3_log_init_default(void);
A3_EXPORT void a3_log_flush(void);

A3_FORMAT_FN(2, 3)
A3_LOG_INLINE void a3_log(A3LogLevel level, char const* fmt, ...) {
    if A3_UNLIKELY (level == A3_LOG_INVALID)
        a3_log_init_default();

    if A3_LIKELY (level < A3_PRIV_LOG_LEVEL)
        return;

#ifdef A3_USE_VA_PACK
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    fprintf(A3_PRIV_LOG_OUTPUT, fmt, __builtin_va_arg_pack());
#pragma GCC diagnostic pop
#else
    va_list args;
    va_start(args, fmt);
    vfprintf(A3_PRIV_LOG_OUTPUT, fmt, args);
    va_end(args);
#endif

    fputc('\n', A3_PRIV_LOG_OUTPUT);
}

A3_H_END

#undef A3_USE_VA_PACK
#undef A3_LOG_INLINE
