/*
 * LOG -- Simple logging utilities.
 *
 * Copyright (c) 2020-2021, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <stdio.h>

#include <a3/cpp.h>
#include <a3/platform/types.h>

A3_H_BEGIN

typedef enum A3LogLevel { TRACE, DEBUG, INFO, WARN, ERROR } A3LogLevel;

A3_EXPORT void a3_log_init(FILE*, A3LogLevel);
A3_EXPORT void a3_log_msg(A3LogLevel, const char*);
A3_EXPORT void a3_log_error(int error, const char* msg);

A3_EXPORT void a3_log_fmt(A3LogLevel, const char*, ...);

#define A3_ERR_FMT(fmt, ...)                                                   \
    do {                                                                       \
        a3_log_fmt(ERROR, "%s (%d): " fmt, __FILE__, __LINE__, __VA_ARGS__);   \
    } while (0)

#define A3_ERR(msg)                                                            \
    do {                                                                       \
        A3_ERR_FMT("%s", (msg));                                               \
    } while (0)

A3_H_END
