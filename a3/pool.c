/*
 * POOL -- A pool allocator for high-traffic objects.
 *
 * Copyright (c) 2020-2021, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#include <a3/shim/aligned_alloc.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <a3/pool.h>
#include <a3/util.h>

static inline size_t align_down(size_t v, size_t align) { return v & ~(align - 1); }
static inline size_t align_up(size_t v, size_t align) { return align_down(v + align - 1, align); }

#define SLOT_OFFSET(POOL)    (align_up((POOL)->obj_size, alignof(A3PoolSlot)))
#define SLOT_OBJ(POOL, SLOT) ((void*)((uintptr_t)(SLOT)-SLOT_OFFSET(POOL)))
#define OBJ_SLOT(POOL, OBJ)  ((A3PoolSlot*)((uintptr_t)(OBJ) + SLOT_OFFSET(POOL)))

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

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
A3Pool* a3_pool_new(size_t obj_size, size_t blocks, size_t align, bool zero_blocks,
                    A3PoolCallback init_cb, A3PoolCallback free_cb) {
    size_t slot_offset = align_up(obj_size, alignof(A3PoolSlot));
    size_t block_size  = align_up(slot_offset + sizeof(A3PoolSlot), align);

    A3Pool* ret;
    A3_UNWRAPN(ret, calloc(1, sizeof(A3Pool)));
    ret->zero_blocks = zero_blocks;
    ret->free_cb     = free_cb;
    ret->obj_size    = obj_size;
    ret->cap         = blocks * block_size;
    A3_UNWRAPN(ret->data, a3_shim_aligned_alloc(ret->cap, align));
    memset(ret->data, 0, ret->cap);
    ret->free = OBJ_SLOT(ret, ret->data);

    uintptr_t base = (uintptr_t)ret->data;
    for (size_t i = 0; i < blocks - 1; i++) {
        A3PoolSlot* slot = (A3PoolSlot*)(base + block_size * i + SLOT_OFFSET(ret));
        slot->next       = (A3PoolSlot*)(base + (i + 1) * block_size + SLOT_OFFSET(ret));
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
    slot->next       = pool->free;
    pool->free       = slot;
}

void a3_pool_free(A3Pool* pool) {
    assert(pool);

    if (pool->free_cb)
        for (A3PoolSlot* current = pool->free; current; current = current->next)
            pool->free_cb(SLOT_OBJ(pool, current));

    a3_shim_aligned_free(pool->data);
    free(pool);
}
