/*
 * POOL -- A pool allocator for high-traffic objects.
 *
 * Copyright (c) 2020, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>

#ifndef _WIN32
#include <stdalign.h>
#else
#define alignof __alignof
#endif

#include <a3/cpp.h>
#include <a3/platform/types.h>

H_BEGIN

#define POOL_ZERO_BLOCKS     true
#define POOL_PRESERVE_BLOCKS false

typedef struct Pool     Pool;
typedef struct PoolSlot PoolSlot;

typedef void (*PoolFreeCallback)(PoolSlot*);

EXPORT Pool* pool_new(size_t block_size, size_t blocks, size_t align,
                      bool zero_blocks, PoolFreeCallback free_cb);
EXPORT void* pool_alloc_block(Pool*);
EXPORT void  pool_free_block(Pool*, void*);
EXPORT void  pool_free(Pool*);

#define POOL_OF(TY, COUNT, ZB, CB)                                             \
    pool_new(sizeof(TY), (COUNT), alignof(TY), (ZB), (CB))

H_END
