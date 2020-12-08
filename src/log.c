/*
 * LOG -- Simple logging utilities.
 *
 * Copyright (c) 2020, Alex O'Brien
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#include <a3/log.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <a3/util.h>

static FILE*    log_out   = NULL;
static LogLevel log_level = ERROR;

void log_init(FILE* out, LogLevel level) {
    log_out   = out;
    log_level = level;
}

FORMAT_FN(2, 3)
void log_fmt(LogLevel level, const char* fmt, ...) {
    if (level < log_level)
        return;

    va_list args;
    va_start(args, fmt);
    vfprintf(log_out, fmt, args);
    fputc('\n', log_out);
    va_end(args);
}

void log_msg(LogLevel level, const char* msg) { log_fmt(level, "%s", msg); }

void log_error(int error, const char* msg) {
    log_fmt(ERROR, "Error: %s (%s).", strerror(error), msg);
}
