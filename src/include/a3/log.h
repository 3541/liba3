/*
 * LOG -- Simple logging utilities.
 *
 * Copyright (c) 2020-2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <stdio.h>

#include <a3/cpp.h>
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
#define A3_ERROR_F                 A3_LOG_F
#define A3_ERRNO_F(CODE, FMT, ...) A3_LOG_F("%s (%d) " FMT, strerror(CODE), CODE, __VA_ARGS__)
#define A3_ERROR                   A3_LOG
#define A3_ERRNO(CODE, MSG)        A3_LOG_F("%s (%d)", strerror(CODE), CODE)
#endif

A3_H_END
