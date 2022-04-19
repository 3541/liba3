/*
 * STRERROR SHIM -- Cross-platform shim for strerror.
 *
 * Copyright (c) 2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

#include <a3/shim/strerror.h>
#include <string.h>

char* a3_shim_strerror(int errnum, char* buf, size_t len) {
#ifdef A3_HAVE_STRERROR_R
#ifdef A3_HAVE_GNU_STRERROR_R
    return strerror_r(errnum, buf, len);
#else
    // XSI
    strerror_r(errnum, buf, len);
    return buf;
#endif
#elif defined(A3_HAVE_STRERROR_S)
    strerror_s(buf, len, errnum);
    return buf;
#endif
}
