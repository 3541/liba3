/*
 * LOG -- Simple logging utilities.
 *
 * Copyright (c) 2020-2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#include <a3/log.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <a3/util.h>

static FILE* LOG_OUTPUT = NULL;

void a3_log_init(FILE* out) { LOG_OUTPUT = out; }

A3_FORMAT_FN(1, 2)
void a3_log(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(LOG_OUTPUT, fmt, args);
    fputc('\n', LOG_OUTPUT);
    va_end(args);
}
