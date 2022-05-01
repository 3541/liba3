/*
 * STRNCASECMP SHIM -- Cross-platform shim for strncasecmp.
 *
 * Copyright (c) 2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

#include <a3/shim/strncasecmp.h>

#ifdef A3_HAVE_STRNCASECMP
#include <strings.h>
#elif defined(A3_HAVE__STRNICMP)
#include <string.h>
#else
#error "No case-insensitive string comparison function available."
#endif

int a3_shim_strncasecmp(char const* lhs, char const* rhs, size_t n) {
#ifdef A3_HAVE_STRNCASECMP
    return strncasecmp(lhs, rhs, n);
#elif defined(A3_HAVE__STRNICMP)
    return _strnicmp(lhs, rhs, n);
#endif
}
