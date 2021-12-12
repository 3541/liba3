/*
 * POOL -- A pool allocator for high-traffic objects.
 *
 * Copyright (c) 2020-2021, Alex O'Brien <3541ax@gmail.com>
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

struct A3PoolSlot {
    struct A3PoolSlot* next;
};

struct A3Pool {
    void*              data;
    A3PoolSlot*        free;
    size_t             block_size;
    size_t             cap; // In bytes.
    bool               zero_blocks;
    A3PoolFreeCallback free_cb;
};

static inline size_t align_down(size_t v, size_t align) { return v & ~(align - 1); }

static inline size_t align_up(size_t v, size_t align) { return align_down(v + align - 1, align); }

A3Pool* a3_pool_new(size_t block_size, size_t blocks, size_t align, bool zero_blocks,
                    A3PoolFreeCallback free_cb) {
    if (block_size < sizeof(A3PoolSlot))
        A3_PANIC_FMT("Block size %zu is too small for a pool slot (%zu).", block_size,
                     sizeof(A3PoolSlot));

    align      = MAX(align, alignof(A3PoolSlot));
    block_size = align_up(block_size, align);

    A3Pool* ret;
    A3_UNWRAPN(ret, calloc(1, sizeof(A3Pool)));
    ret->zero_blocks = zero_blocks;
    ret->free_cb     = free_cb;
    ret->block_size  = block_size;
    ret->cap         = blocks * block_size;
#ifndef _WIN32
    A3_UNWRAPSD(posix_memalign(&ret->data, align, ret->cap));
#else
    A3_UNWRAPN(ret->data, _aligned_malloc(ret->cap, align));
#endif
    memset(ret->data, 0, ret->cap);
    ret->free = ret->data;

    uintptr_t base = (uintptr_t)ret->data;
    for (size_t i = 0; i < blocks - 1; i++) {
        *(uintptr_t*)(base + block_size * i) = (i + 1) * block_size + base;
    }
    return ret;
}

void* a3_pool_alloc_block(A3Pool* pool) {
    assert(pool);

    if (!pool->free)
        return NULL;
    A3PoolSlot* slot = pool->free;

    pool->free = slot->next;
    slot->next = NULL;

    return (void*)slot;
}

void a3_pool_free_block(A3Pool* pool, void* block) {
    assert(pool);
    assert(block);

    A3PoolSlot* slot = block;
    if (pool->zero_blocks)
        memset(slot, 0, pool->block_size);

    slot->next       = pool->free;
    pool->free       = slot;
}

void a3_pool_free(A3Pool* pool) {
    assert(pool);

    if (pool->free_cb)
        for (A3PoolSlot* current = pool->free; current; current = current->next)
            pool->free_cb(current);

#ifndef _WIN32
    free(pool->data);
#else
    // _aligned_malloced memory cannot be freed with free.
    _aligned_free(pool->data);
#endif
    free(pool);
}
