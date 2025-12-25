/*
 * LOG -- Simple logging utilities.
 *
 * Copyright (c) 2020-2022, 2024-2025, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#include "a3/log.h"

#include <stdio.h>
#include <stdlib.h>

#include "a3/shim/likely.h"
#include "a3/str.h"

FILE*      A3_DETAIL_LOG_OUTPUT = NULL;
A3LogLevel A3_DETAIL_LOG_LEVEL  = A3_LOG_INVALID;

void a3_log_init(FILE* out, A3LogLevel level) {
    A3CString env = a3_cstring_from(getenv("A3_LOG_LEVEL"));

    if (level == A3_LOG_INVALID)
        level = A3_LOG_DEBUG;

    if (env.ptr) {
        if (a3_string_cmpi(env, A3_CS("trace")) == 0)
            level = A3_LOG_TRACE;
        else if (a3_string_cmpi(env, A3_CS("debug")) == 0)
            level = A3_LOG_DEBUG;
        else if (a3_string_cmpi(env, A3_CS("info")) == 0)
            level = A3_LOG_INFO;
        else if (a3_string_cmpi(env, A3_CS("warn")) == 0)
            level = A3_LOG_WARN;
        else if (a3_string_cmpi(env, A3_CS("error")) == 0)
            level = A3_LOG_ERROR;
    }

    A3_DETAIL_LOG_OUTPUT = out;
    A3_DETAIL_LOG_LEVEL  = level;
}

void a3_log_init_default(void) { a3_log_init(stderr, A3_LOG_INVALID); }

void a3_log_flush(void) {
    if A3_UNLIKELY (A3_DETAIL_LOG_LEVEL == A3_LOG_INVALID)
        a3_log_init_default();

    assert(A3_DETAIL_LOG_OUTPUT);
    (void)fflush(A3_DETAIL_LOG_OUTPUT);
}
