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

#include <a3/util.h>

typedef struct PoolSlot {
    struct PoolSlot* next;
} PoolSlot;

struct Pool {
    void*     data;
    PoolSlot* free;
    size_t    block_size;
    size_t    capacity; // In bytes.
};

Pool* pool_new(size_t block_size, size_t blocks) {
    if (block_size < sizeof(PoolSlot))
        PANIC_FMT("Block size %zu is too small for a pool slot (%zu).",
                  block_size, sizeof(PoolSlot));

    Pool* ret;
    UNWRAPN(ret, calloc(1, sizeof(Pool)));
    ret->block_size = block_size;
    ret->capacity   = blocks * block_size;
    UNWRAPN(ret->data, calloc(blocks, block_size));
    ret->free = ret->data;

    uintptr_t base = (uintptr_t)ret->data;
    for (size_t i = 0; i < blocks - 1; i++) {
        *(uintptr_t*)(ret->data + block_size * i) = (i + 1) * block_size + base;
    }
    return ret;
}

void* pool_alloc_block(Pool* pool) {
    assert(pool);

    if (!pool->free)
        return NULL;
    PoolSlot* slot = pool->free;

    pool->free = slot->next;

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

    free(pool->data);
    free(pool);
}
