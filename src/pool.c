/*
 * POOL -- A pool allocator for high-traffic objects.
 *
 * Copyright (c) 2020, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#include <a3/pool.h>

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <stdalign.h>
#else
#define alignof __alignof
#endif

#include <a3/util.h>

typedef struct PoolSlot {
    struct PoolSlot* next;
} PoolSlot;

struct Pool {
    void*     data;
    PoolSlot* free;
    size_t    block_size;
    size_t    cap; // In bytes.
};

static inline size_t align_down(size_t v, size_t align) {
    return v & ~(align - 1);
}

static inline size_t align_up(size_t v, size_t align) {
    return align_down(v + align - 1, align);
}

Pool* pool_new(size_t block_size, size_t blocks, size_t align) {
    if (block_size < sizeof(PoolSlot))
        PANIC_FMT("Block size %zu is too small for a pool slot (%zu).",
                  block_size, sizeof(PoolSlot));

    align      = MAX(align, alignof(PoolSlot));
    block_size = align_up(block_size, align);

    Pool* ret;
    UNWRAPN(ret, calloc(1, sizeof(Pool)));
    ret->block_size = block_size;
    ret->cap        = blocks * block_size;
#ifndef _WIN32
    UNWRAPSD(posix_memalign(&ret->data, align, ret->cap));
#else
    UNWRAPN(ret->data, _aligned_malloc(ret->cap, align));
#endif
    memset(ret->data, 0, ret->cap);
    ret->free = ret->data;

    uintptr_t base = (uintptr_t)ret->data;
    for (size_t i = 0; i < blocks - 1; i++) {
        *(uintptr_t*)(base + block_size * i) = (i + 1) * block_size + base;
    }
    return ret;
}

void* pool_alloc_block(Pool* pool) {
    assert(pool);

    if (!pool->free)
        return NULL;
    PoolSlot* slot = pool->free;

    pool->free = slot->next;
    memset(slot, 0, pool->block_size);

    return (void*)slot;
}

void pool_free_block(Pool* pool, void* block) {
    assert(pool);
    assert(block);

    PoolSlot* slot = block;
    slot->next     = pool->free;
    pool->free     = slot;
}

void pool_free(Pool* pool) {
    assert(pool);

#ifndef _WIN32
    free(pool->data);
#else
    // _aligned_alloced memory cannot be freed with free.
    _aligned_free(pool->data);
#endif
    free(pool);
}
