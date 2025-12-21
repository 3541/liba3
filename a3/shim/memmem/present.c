/*
 * MEMMEM SHIM -- Cross-platform shim for memmem.
 *
 * Copyright (c) 2022, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

#define _GNU_SOURCE

#include <stddef.h>
#include <string.h>

#include <a3/shim/memmem.h>

void* a3_shim_memmem(void const* haystack, size_t haystack_len, void const* needle,
                     size_t needle_len) {
    return memmem(haystack, haystack_len, needle, needle_len);
}
