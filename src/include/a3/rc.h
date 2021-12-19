/*
 * RC -- Reference counting.
 *
 * Copyright (c) 2021, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

/// \file rc.h
/// # Reference Counting
/// Provides facilities to make an object reference-counted. See ::A3_REFCOUNTED.
////
/// See also rc.hh for a convenient C++ interface.

#pragma once

#include <assert.h>
#include <stdint.h>

#include <a3/cpp.h>
#include <a3/types.h>

#ifdef __cplusplus
#define A3_REFCOUNTED_T(T)                                                                         \
    T _ref_count { 0 }
#else
/// Make an object reference counted, using a specific type for the reference count. The given type
/// must support at least the following:
/// * Initialization with an integer.
/// * Increment and decrement operators.
/// * Comparison with zero.
#define A3_REFCOUNTED_T(T) T _ref_count
#endif

/// To make an object reference counted, place A3_REFCOUNTED somewhere in its definition. If a
/// specific type for the reference count is required, use ::A3_REFCOUNTED_T instead (the default is
/// uint32_t). Then, call ::A3_REF_INIT in the constructor to initialize the refcount to 1.
/// Increment the refcount as appropriate using ::A3_REF, and decrement it using ::A3_UNREF.
/// ::A3_UNREF allows a second argument which is a destructor, called if the reference count reaches
/// 0.
#define A3_REFCOUNTED A3_REFCOUNTED_T(uint32_t)

/// Get the current reference count of the object `O`.
#define A3_REF_COUNT(O) (O->_ref_count)

/// Initialize the reference count to 1.
#define A3_REF_INIT(O) A3_REF_COUNT(O) = 1

/// Acquire a reference to `O`.
#define A3_REF(O) A3_REF_COUNT(O)++

/// Decrement the reference count, calling function-like `D` if the count reaches zero.
#define A3_UNREF_D(O, D)                                                                           \
    do {                                                                                           \
        assert(A3_REF_COUNT(O) > 0);                                                               \
        if (--A3_REF_COUNT(O) == 0)                                                                \
            D(O);                                                                                  \
    } while (0)

/// Decrement the reference count.
#define A3_UNREF(O)                                                                                \
    do {                                                                                           \
        assert(A3_REF_COUNT(O) > 0);                                                               \
        --A3_REF_COUNT(O);                                                                         \
    } while (0)
