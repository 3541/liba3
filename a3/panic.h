/*
 * PANIC — Panic macros.
 *
 * Copyright (c) 2020-2022, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

#pragma once

#include "a3/detail/log.h"

// Abort with a message.
#define A3_PANIC_FMT(FMT, ...)                                                                     \
    do {                                                                                           \
        a3_log(A3_LOG_ERROR, "PANIC %s (%d): " FMT, __FILE__, __LINE__, __VA_ARGS__);              \
        a3_log_flush();                                                                            \
        abort();                                                                                   \
    } while (0)

#define A3_PANIC(msg) A3_PANIC_FMT("%s", (msg))

#define A3_UNREACHABLE() A3_PANIC("UNREACHABLE")
