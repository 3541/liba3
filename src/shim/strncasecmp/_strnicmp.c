/*
 * STRNCASECMP SHIM -- Cross-platform shim for strncasecmp.
 *
 * Copyright (c) 2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

#include <a3/shim/strncasecmp.h>
#include <string.h>

int a3_shim_strncasecmp(char const* lhs, char const* rhs, size_t n) {
    return _strnicmp(lhs, rhs, n);
}
