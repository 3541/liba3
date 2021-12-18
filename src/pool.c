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

#define SLOT_OBJ(POOL, SLOT) ((void*)((uintptr_t)(SLOT) - (POOL)->obj_size))
#define OBJ_SLOT(POOL, OBJ) ((A3PoolSlot*)((uintptr_t)(OBJ) + (POOL)->obj_size))

struct A3PoolSlot {
    struct A3PoolSlot* next;
};

struct A3Pool {
    void*          data;
    A3PoolSlot*    free;
    size_t         obj_size;
    size_t         cap; // In bytes.
    bool           zero_blocks;
    A3PoolCallback free_cb;
};

static inline size_t align_down(size_t v, size_t align) { return v & ~(align - 1); }

static inline size_t align_up(size_t v, size_t align) { return align_down(v + align - 1, align); }

A3Pool* a3_pool_new(size_t obj_size, size_t blocks, size_t align, bool zero_blocks,
                    A3PoolCallback init_cb, A3PoolCallback free_cb) {
    align             = MAX(align, alignof(A3PoolSlot));
    size_t block_size = align_up(obj_size + sizeof(A3PoolSlot), align);

    A3Pool* ret;
    A3_UNWRAPN(ret, calloc(1, sizeof(A3Pool)));
    ret->zero_blocks = zero_blocks;
    ret->free_cb     = free_cb;
    ret->obj_size    = obj_size;
    ret->cap         = blocks * block_size;
#ifndef _WIN32
    A3_UNWRAPSD(posix_memalign(&ret->data, align, ret->cap));
#else
    A3_UNWRAPN(ret->data, _aligned_malloc(ret->cap, align));
#endif
    memset(ret->data, 0, ret->cap);
    ret->free = (A3PoolSlot*)((uintptr_t)ret->data + obj_size);

    uintptr_t base = (uintptr_t)ret->data;
    for (size_t i = 0; i < blocks - 1; i++) {
        A3PoolSlot* slot = (A3PoolSlot*)(base + block_size * i + obj_size);
        slot->next       = (A3PoolSlot*)(base + (i + 1) * block_size + obj_size);
        if (init_cb)
            init_cb(SLOT_OBJ(ret, slot));
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

    return SLOT_OBJ(pool, slot);
}

void a3_pool_free_block(A3Pool* pool, void* block) {
    assert(pool);
    assert(block);

    if (pool->zero_blocks)
        memset(block, 0, pool->obj_size);

    A3PoolSlot* slot = OBJ_SLOT(pool, block);
    slot->next = pool->free;
    pool->free = slot;
}

void a3_pool_free(A3Pool* pool) {
    assert(pool);

    if (pool->free_cb)
        for (A3PoolSlot* current = pool->free; current; current = current->next)
            pool->free_cb(SLOT_OBJ(pool, current));

#ifndef _WIN32
    free(pool->data);
#else
    // _aligned_malloced memory cannot be freed with free.
    _aligned_free(pool->data);
#endif
    free(pool);
}
