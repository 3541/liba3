/*
 * LOG -- Simple logging utilities.
 *
 * Copyright (c) 2020-2022, 2024, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 *
 * To set a non-default log level, define A3_LOG_LEVEL before including this file. To avoid
 * confusion, this file should not be included multiple times, or in any header.
 */

#pragma once

#include <a3/shim/format.h>
#include <a3/shim/likely.h>
#include <a3/shim/va_fwd.h>
#include <stdarg.h>
#include <stdio.h>

#include <a3/cpp.h>
#include <a3/types.h>

A3_H_BEGIN

typedef int A3LogLevel;
#define A3_LOG_TRACE   0
#define A3_LOG_DEBUG   1
#define A3_LOG_INFO    2
#define A3_LOG_WARN    3
#define A3_LOG_ERROR   4
#define A3_LOG_INVALID (~0)

extern A3LogLevel A3_PRIV_LOG_LEVEL;

A3_EXPORT void a3_log_init(FILE*, A3LogLevel);
A3_EXPORT void a3_log_init_default(void);
A3_EXPORT void a3_log_impl(char const*, A3_VA_FWD_ARG());

A3_FORMAT_FN(2, 3)
A3_VA_FWD_INLINE void a3_log(A3LogLevel level, char const* fmt, ...) {
    if A3_UNLIKELY (level == A3_LOG_INVALID)
        a3_log_init_default();

    if A3_LIKELY (level < A3_PRIV_LOG_LEVEL)
        return;

    A3_VA_FWD_DECL(args, fmt);
    a3_log_impl(fmt, A3_VA_FWD_PASS(args));
}

A3_H_END
