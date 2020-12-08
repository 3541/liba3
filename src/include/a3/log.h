/*
 * LOG -- Simple logging utilities.
 *
 * Copyright (c) 2020, Alex O'Brien
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <stdio.h>

#include <a3/cpp.h>

H_BEGIN

typedef enum LogLevel { TRACE, DEBUG, INFO, WARN, ERROR } LogLevel;

void log_init(FILE*, LogLevel);
void log_msg(LogLevel, const char*);
void log_error(int error, const char* msg);

void log_fmt(LogLevel, const char*, ...);

#define ERR_FMT(fmt, ...)                                                      \
    do {                                                                       \
        log_fmt(ERROR, "%s (%d): " fmt "\n", __FILE__, __LINE__, __VA_ARGS__); \
    } while (0)

#define ERR(msg)                                                               \
    do {                                                                       \
        ERR_FMT("%s", (msg));                                                  \
    } while (0)

H_END
