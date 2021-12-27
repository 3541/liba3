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
#include <a3/types.h>

A3_H_BEGIN

#define A3_POOL_ZERO_BLOCKS     true
#define A3_POOL_PRESERVE_BLOCKS false

typedef struct A3Pool     A3Pool;
typedef struct A3PoolSlot A3PoolSlot;

typedef void (*A3PoolCallback)(void*);

A3_EXPORT A3Pool* a3_pool_new(size_t block_size, size_t blocks, size_t align, bool zero_blocks,
                              A3PoolCallback init_cb, A3PoolCallback free_cb);
A3_EXPORT void*   a3_pool_alloc_block(A3Pool*);
A3_EXPORT void    a3_pool_free_block(A3Pool*, void*);
A3_EXPORT void    a3_pool_free(A3Pool*);

#define A3_POOL_OF(TY, COUNT, ZERO_BLOCKS, INIT_CB, FREE_CB)                                       \
    a3_pool_new(sizeof(TY), (COUNT), alignof(TY), (ZERO_BLOCKS), (INIT_CB), (FREE_CB))

A3_H_END

#ifdef __cplusplus

#include <cassert>

#include <a3/log.h>

// The following are macros which can be used to override `new` and `delete` on a C++ class so that
// is allocated from a pool.
// - Invoke one of the macros A3_POOL_ALLOCATED* at the start of the class.
// - Invoke A3_POOL_STORAGE in _one_ translation unit.

#define A3_POOL_STORAGE(T, COUNT, ZB) A3Pool* T::_POOL = A3_POOL_OF(T, COUNT, ZB, nullptr, nullptr)

#define A3_POOL_MEMBER static A3Pool* _POOL;

#define A3_POOL_OP_NEW(T)                                                                          \
    static void* operator new(size_t size) noexcept {                                              \
        assert(size == sizeof(T));                                                                 \
        (void)size;                                                                                \
        void* ret = a3_pool_alloc_block(_POOL);                                                    \
        if (!ret) {                                                                                \
            a3_log_msg(LOG_ERROR, #T " pool exhausted.");                                          \
            return nullptr;                                                                        \
        }                                                                                          \
        return ret;                                                                                \
    }

#define A3_POOL_OP_DELETE(T)                                                                       \
    static void operator delete(void* ptr) {                                                       \
        assert(ptr);                                                                               \
        a3_pool_free_block(_POOL, ptr);                                                            \
    }

#define A3_POOL_ALLOCATED(T)                                                                       \
private:                                                                                           \
    A3_POOL_MEMBER                                                                                 \
public:                                                                                            \
    A3_POOL_OP_NEW(T)                                                                              \
    A3_POOL_OP_DELETE(T)

#define A3_POOL_ALLOCATED_PRIV_NEW(T)                                                              \
private:                                                                                           \
    A3_POOL_MEMBER                                                                                 \
    A3_POOL_OP_NEW(T)                                                                              \
public:                                                                                            \
    A3_POOL_OP_DELETE(T)

#endif
