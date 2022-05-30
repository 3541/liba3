/*
 * LOG -- Simple logging utilities.
 *
 * Copyright (c) 2020-2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#include <stdarg.h>
#include <stdio.h>

#include <a3/log.h>
#include <a3/util.h>

static FILE*      LOG_OUTPUT = NULL;
static A3LogLevel LOG_LEVEL  = A3_LOG_TRACE;

void a3_log_init(FILE* out, A3LogLevel level) {
    LOG_OUTPUT = out;
    LOG_LEVEL  = level;
}

A3_FORMAT_FN(2, 3)
void a3_log(A3LogLevel level, const char* fmt, ...) {
    if (level < LOG_LEVEL)
        return;
    va_list args;
    va_start(args, fmt);
    vfprintf(LOG_OUTPUT, fmt, args);
    fputc('\n', LOG_OUTPUT);
    va_end(args);
}
