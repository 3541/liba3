/*
 * POOL -- A pool allocator for high-traffic objects.
 *
 * Copyright (c) 2020-2021, Alex O'Brien <3541ax@gmail.com>
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

A3_H_BEGIN

#define A3_POOL_ZERO_BLOCKS     true
#define A3_POOL_PRESERVE_BLOCKS false

typedef struct A3Pool     A3Pool;
typedef struct A3PoolSlot A3PoolSlot;

typedef void (*A3PoolFreeCallback)(A3PoolSlot*);

A3_EXPORT A3Pool* a3_pool_new(size_t block_size, size_t blocks, size_t align,
                              bool zero_blocks, A3PoolFreeCallback free_cb);
A3_EXPORT void*   a3_pool_alloc_block(A3Pool*);
A3_EXPORT void    a3_pool_free_block(A3Pool*, void*);
A3_EXPORT void    a3_pool_free(A3Pool*);

#define A3_POOL_OF(TY, COUNT, ZB, CB)                                          \
    a3_pool_new(sizeof(TY), (COUNT), alignof(TY), (ZB), (CB))

A3_H_END
