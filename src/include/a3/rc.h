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

#include <a3/cpp.h>
#include <a3/macro.h>
#include <a3/platform/types.h>

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

A3_H_BEGIN
static A3_ALLOW_UNUSED inline void _a3_unref_nop_dest(void* p) { (void)p; }
A3_H_END

#define _A3_UNREF(O, D)                                                                            \
    do {                                                                                           \
        assert(A3_REF_COUNT(O) > 0);                                                               \
        if (--A3_REF_COUNT(O) == 0)                                                                \
            D(O);                                                                                  \
    } while (0)

#define _A3_UNREF_DEST(O, D) _A3_UNREF(O, D)
#define _A3_UNREF_NODEST(O)  _A3_UNREF(O, _a3_unref_nop_dest)

#define _A3_UNREF_OPTIONS(...) A3_ARG3(__VA_ARGS__, _A3_UNREF_DEST, _A3_UNREF_NODEST)

#define A3_UNREF(...) _A3_UNREF_OPTIONS(__VA_ARGS__)(__VA_ARGS__)
