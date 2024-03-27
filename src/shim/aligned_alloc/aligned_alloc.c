/*
 * ALIGNED_ALLOC SHIM -- Cross-platform shim for aligned_alloc.
 *
 * Copyright (c) 2022, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 *
 * Windows does not provide aligned_alloc.
 */

#include <a3/shim/aligned_alloc.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

void* a3_shim_aligned_alloc(size_t size, size_t align) {
    assert(size > 0);
    assert(align > 0);

    void* ret = aligned_alloc(align, size);

    // Valgrind doesn't support aligned_alloc(), and just returns NULL from all calls.
    if (!ret) {
        ret = malloc(size + align);
        ret = (void*)(((uintptr_t)ret + align - 1) & ~(align - 1));
    }

    return ret;
}

void a3_shim_aligned_free(void* ptr) {
    assert(ptr);

    free(ptr);
}
