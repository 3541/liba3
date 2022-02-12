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

A3_EXPORT void a3_log_init(FILE*, A3LogLevel);
A3_EXPORT void a3_log(A3LogLevel, char const*, ...); // NOLINT(readability-redundant-declaration)

A3_H_END
