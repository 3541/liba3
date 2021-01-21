/*
 * LOG -- Simple logging utilities.
 *
 * Copyright (c) 2020-2021, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#include <a3/log.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <a3/util.h>

static FILE*      log_out   = NULL;
static A3LogLevel log_level = ERROR;

void a3_log_init(FILE* out, A3LogLevel level) {
    log_out   = out;
    log_level = level;
}

A3_FORMAT_FN(2, 3)
void a3_log_fmt(A3LogLevel level, const char* fmt, ...) {
    if (level < log_level)
        return;

    va_list args;
    va_start(args, fmt);
    vfprintf(log_out, fmt, args);
    fputc('\n', log_out);
    va_end(args);
}

void a3_log_msg(A3LogLevel level, const char* msg) {
    a3_log_fmt(level, "%s", msg);
}

void a3_log_error(int error, const char* msg) {
    a3_log_fmt(ERROR, "Error: %s (%s).", msg, strerror(error));
}
