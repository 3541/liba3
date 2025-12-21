/*
 * STRERROR SHIM -- Cross-platform shim for strerror.
 *
 * Copyright (c) 2022, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

#include "a3/shim/platform.h"

#ifdef A3_PLATFORM_OS_UNIXLIKE

#include "a3/shim/strerror.h"

#undef _GNU_SOURCE

#include <string.h>

char* a3_shim_strerror(int errnum, char* buf, size_t len) {
    strerror_r(errnum, buf, len);
    return buf;
}

#endif
