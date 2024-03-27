/*
 * MEMMEM SHIM -- Cross-platform shim for memmem.
 *
 * Copyright (c) 2022, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 *
 * Windows does not provide memmem.
 */

#include <a3/shim/memmem.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void* a3_shim_memmem(void const* haystack, size_t haystack_len, void const* needle,
                     size_t needle_len) {
    if (!haystack || !haystack_len || !needle || !needle_len)
        return NULL;

    for (uint8_t const* sp = haystack; sp + needle_len < (uint8_t const*)haystack + haystack_len;
         sp++) {
        if (memcmp(sp, needle, needle_len) == 0)
            return (void*)sp;
    }

    return NULL;
}
