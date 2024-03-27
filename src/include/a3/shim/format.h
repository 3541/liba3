/*
 * FORMAT — Cross-platform shim for format hint attributes.
 *
 * Copyright (c) 2020, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

#pragma once

#if defined(__GNUC__) || defined(__clang__)
#define A3_FORMAT_FN(FMT_INDEX, VARG_INDEX)                                                        \
    __attribute__((__format__(__printf__, FMT_INDEX, VARG_INDEX)))
#else
#define A3_FORMAT_FN(FMT_INDEX, VARG_INDEX)
#endif
