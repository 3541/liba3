/*
 * POOL -- A pool allocator for high-traffic objects.
 *
 * Copyright (c) 2020, Alex O'Brien
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <stddef.h>

struct Pool;
typedef struct Pool Pool;

Pool* pool_new(size_t block_size, size_t blocks);
void* pool_alloc_block(Pool*);
void  pool_free_block(Pool*, void*);
void  pool_free(Pool*);
