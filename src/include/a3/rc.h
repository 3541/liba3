/*
 * RC -- Reference counting.
 *
 * Copyright (c) 2021, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <assert.h>
#include <stdint.h>

// To make an object reference counted, place A3_REFCOUNTED somewhere in its definition. If a
// specific type for the reference count is required, use A3_REFCOUNTED_T instead (the default is
// uint32_t). Then, call A3_REF_INIT in the constructor to initialize the refcount to 1. Increment
// the refcount as appropriate using A3_REF, and decrement it using A3_UNREF. A3_UNREF allows a
// second argument which is a destructor, called if the reference count reaches 0.

#ifdef __cplusplus
#define A3_REFCOUNTED_T(T)                                                                         \
    T _ref_count { 0 }
#else
#define A3_REFCOUNTED_T(T) T _ref_count
#endif
#define A3_REFCOUNTED A3_REFCOUNTED_T(uint32_t)

#define A3_REF_COUNT(O) (O->_ref_count)

#define A3_REF_INIT(O) A3_REF_COUNT(O) = 1
#define A3_REF(O)      A3_REF_COUNT(O)++
#define A3_UNREF(O, ...)                                                                           \
    do {                                                                                           \
        assert(A3_REF_COUNT(O) > 0);                                                               \
        if (--A3_REF_COUNT(O) == 0)                                                                \
            (void)__VA_ARGS__(O);                                                                  \
    } while (0)