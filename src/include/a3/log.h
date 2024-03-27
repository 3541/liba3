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
 *
 * A runtime log level may also be set at initialization time. This will not, however, cause
 * messages logged at a level below the compile-time filter to appear.
 */

/// \file log.h
/// # Logging
/// Simple logging to an output stream.

#ifndef A3_LOG_H
#define A3_LOG_H

#include <a3/shim/strerror.h>
#include <stdio.h>

#include <a3/cpp.h>
#include <a3/log_internal.h>
#include <a3/macro.h>
#include <a3/types.h>

A3_H_BEGIN

#ifndef A3_LOG_LEVEL
/// The compile-time log level. Anything less severe is compiled out.
#define A3_LOG_LEVEL A3_LOG_TRACE
#endif

/// Log a formatted message at the given level.
#define A3_LOG_F(LV, FMT, ...) a3_log((LV), "%s (%d): " FMT, __FILE__, __LINE__, __VA_ARGS__)
/// Log a message at the given level.
#define A3_LOG(LV, MSG) A3_LOG_F((LV), "%s", MSG)
/// Log a formatted message at the A3_LOG_TRACE level.
#define A3_TRACE_F(...)
/// Log a formatted message at the A3_LOG_DEBUG level.
#define A3_DEBUG_F(...)
/// Log a formatted message at the A3_LOG_INFO level.
#define A3_INFO_F(...)
/// Log a formatted message at the A3_LOG_WARN level.
#define A3_WARN_F(...)
/// Log a formatted message at the A3_LOG_ERROR level.
#define A3_ERROR_F(...)
/// Log a formatted message, plus errno string, at the A3_LOG_ERROR level.
#define A3_ERRNO_F(...)
/// Log a message at the A3_LOG_TRACE level.
#define A3_TRACE(MSG)
/// Log a message at the A3_LOG_DEBUG level.
#define A3_DEBUG(MSG)
/// Log a message at the A3_LOG_INFO level.
#define A3_INFO(MSG)
/// Log a message at the A3_LOG_WARN level.
#define A3_WARN(MSG)
/// Log a message at the A3_LOG_ERROR level.
#define A3_ERROR(MSG)
/// Log a message, plus errno string, at the A3_LOG_ERROR level.
#define A3_ERRNO(CODE, MSG)

#ifndef DOXYGEN
#if A3_LOG_LEVEL <= A3_LOG_TRACE
#undef A3_TRACE_F
#undef A3_TRACE
#define A3_TRACE_F(FMT, ...) A3_LOG_F(A3_LOG_TRACE, FMT, __VA_ARGS__)
#define A3_TRACE(MSG)        A3_LOG(A3_LOG_TRACE, (MSG))
#endif

#if A3_LOG_LEVEL <= A3_LOG_DEBUG
#undef A3_DEBUG_F
#undef A3_DEBUG
#define A3_DEBUG_F(FMT, ...) A3_LOG_F(A3_LOG_DEBUG, FMT, __VA_ARGS__)
#define A3_DEBUG(MSG)        A3_LOG(A3_LOG_DEBUG, (MSG))
#endif

#if A3_LOG_LEVEL <= A3_LOG_INFO
#undef A3_INFO_F
#undef A3_INFO
#define A3_INFO_F(FMT, ...) A3_LOG_F(A3_LOG_INFO, FMT, __VA_ARGS__)
#define A3_INFO(MSG)        A3_LOG(A3_LOG_INFO, (MSG))
#endif

#if A3_LOG_LEVEL <= A3_LOG_WARN
#undef A3_WARN_F
#undef A3_WARN
#define A3_WARN_F(FMT, ...) A3_LOG_F(A3_LOG_WARN, FMT, __VA_ARGS__)
#define A3_WARN(MSG)        A3_LOG(A3_LOG_WARN, (MSG))
#endif

#if A3_LOG_LEVEL <= A3_LOG_ERROR
#undef A3_ERROR_F
#undef A3_ERRNO_F
#undef A3_ERROR
#undef A3_ERRNO
#define A3_ERROR_F(FMT, ...) A3_LOG_F(A3_LOG_ERROR, FMT, __VA_ARGS__)
#define A3_ERROR(MSG)        A3_LOG(A3_LOG_ERROR, (MSG))
#define A3_ERRNO_F(CODE, FMT, ...)                                                                 \
    do {                                                                                           \
        char A3_M_PASTE(buf_, __LINE__)[32] = { 0 };                                               \
        A3_LOG_F(A3_LOG_ERROR, "%s (%d) " FMT,                                                     \
                 a3_shim_strerror((CODE), A3_M_PASTE(buf_, __LINE__),                              \
                                  sizeof(A3_M_PASTE(buf_, __LINE__))),                             \
                 (CODE), __VA_ARGS__);                                                             \
    } while (0)
#define A3_ERRNO(CODE, MSG) A3_ERRNO_F((CODE), "%s", (MSG))
#endif
#endif

A3_H_END

#else
#ifndef A3_LOG_IGNORE_MULTIPLE_INCLUSION
#warning "a3/log.h included multiple times. Log level settings may be confused."
#endif
#endif
