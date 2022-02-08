/*
 * LOG -- Simple logging utilities.
 *
 * Copyright (c) 2020-2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 *
 * To set a non-default log level, define A3_LOG_LEVEL before including this file. To avoid
 * confusion, this file should not be included multiple times, or in any header.
 */

#ifndef A3_LOG_H
#define A3_LOG_H

#include <stdio.h>

#include <a3/cpp.h>
#include <a3/macro.h>
#include <a3/types.h>

A3_H_BEGIN

typedef int A3LogLevel;
#define A3_LOG_TRACE 0
#define A3_LOG_DEBUG 1
#define A3_LOG_INFO  2
#define A3_LOG_WARN  3
#define A3_LOG_ERROR 4

#ifndef A3_LOG_LEVEL
#define A3_LOG_LEVEL A3_LOG_INFO
#endif

A3_EXPORT void a3_log_init(FILE*);
A3_EXPORT void a3_log(char const*, ...);

#define A3_LOG_F(FMT, ...) a3_log("%s (%d): " FMT, __FILE__, __LINE__, __VA_ARGS__)
#define A3_LOG(MSG)        A3_LOG_F("%s", MSG)
#define A3_TRACE_F(...)
#define A3_DEBUG_F(...)
#define A3_WARN_F(...)
#define A3_ERROR_F(...)
#define A3_ERRNO_F(...)
#define A3_TRACE(MSG)
#define A3_DEBUG(MSG)
#define A3_WARN(MSG)
#define A3_ERROR(MSG)
#define A3_ERRNO(CODE, MSG)

#if A3_LOG_LEVEL <= A3_LOG_TRACE
#undef A3_TRACE_F
#undef A3_TRACE
#define A3_TRACE_F A3_LOG_F
#define A3_TRACE   A3_LOG
#endif

#if A3_LOG_LEVEL <= A3_LOG_DEBUG
#undef A3_DEBUG_F
#undef A3_DEBUG
#define A3_DEBUG_F A3_LOG_F
#define A3_DEBUG   A3_LOG
#endif

#if A3_LOG_LEVEL <= A3_LOG_WARN
#undef A3_WARN_F
#undef A3_WARN
#define A3_WARN_F A3_LOG_F
#define A3_WARN   A3_LOG
#endif

#if A3_LOG_LEVEL <= A3_LOG_ERROR
#undef A3_ERROR_F
#undef A3_ERRNO_F
#undef A3_ERROR
#undef A3_ERRNO
#define A3_ERROR_F A3_LOG_F
#ifndef _GNU_SOURCE
#ifdef _MSC_VER
#define strerror_r(CODE, BUF, LEN) strerror_s((BUF), (LEN), (CODE))
#endif
#define A3_ERRNO_F(CODE, FMT, ...)                                                                 \
    do {                                                                                           \
        char A3_M_PASTE(buf_, __LINE__)[32] = { 0 };                                               \
        strerror_r((CODE), A3_M_PASTE(buf_, __LINE__), sizeof(A3_M_PASTE(buf_, __LINE__)));        \
        A3_LOG_F("%s (%d) " FMT, A3_M_PASTE(buf_, __LINE__), (CODE), __VA_ARGS__);                 \
    } while (0)
#else
#define A3_ERRNO_F(CODE, FMT, ...)                                                                 \
    do {                                                                                           \
        char A3_M_PASTE(buf_, __LINE__)[32] = { 0 };                                               \
        A3_LOG_F(                                                                                  \
            "%s (%d) " FMT,                                                                        \
            strerror_r((CODE), A3_M_PASTE(buf_, __LINE__), sizeof(A3_M_PASTE(buf_, __LINE__))),    \
            (CODE), __VA_ARGS__);                                                                  \
    } while (0)
#endif
#define A3_ERROR            A3_LOG
#define A3_ERRNO(CODE, MSG) A3_ERRNO_F((CODE), "%s", (MSG))
#endif

A3_H_END

#else
#ifndef A3_LOG_IGNORE_MULTIPLE_INCLUSION
#warn "a3/log.h included multiple times. Log level settings may be confused."
#endif
#endif
