/*
 * POOL -- A pool allocator for high-traffic objects.
 *
 * Copyright (c) 2020, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <stddef.h>

#ifndef _WIN32
#include <stdalign.h>
#else
#define alignof __alignof
#endif

#include <a3/cpp.h>
#include <a3/platform.h>

H_BEGIN

typedef struct Pool Pool;

EXPORT Pool* pool_new(size_t block_size, size_t blocks, size_t align);
EXPORT void* pool_alloc_block(Pool*);
EXPORT void  pool_free_block(Pool*, void*);
EXPORT void  pool_free(Pool*);

#define POOL_OF(TY, COUNT) pool_new(sizeof(TY), (COUNT), alignof(TY))

H_END
