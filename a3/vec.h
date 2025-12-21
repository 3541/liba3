/*
 * VEC -- A type-generic vector.
 *
 * Copyright (c) 2020-2022, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

/// \file vec.h
/// # Vec
/// A type-generic vector container. Similar to std::vector<T> in C++ or Vec<T> in Rust. For the
/// most part, this library imitates (a subset of) the Rust API. Mutating methods WILL invalidate
/// pointers to elements.

#pragma once

#include <assert.h>
#include <stdalign.h>
#include <stddef.h>
#include <string.h>

#include <a3/cpp.h>
#include <a3/macro.h>
#include <a3/types.h>

A3_H_BEGIN

typedef struct A3Vec {
    size_t cap;
    size_t len;
    size_t elem_size;
    size_t elem_align;
    void*  buf;
} A3Vec;

A3_EXPORT void  a3_vec_init_(A3Vec*, size_t elem_size, size_t elem_align, size_t cap);
A3_EXPORT void* a3_vec_write_ptr_(A3Vec*);
A3_EXPORT void* a3_vec_pop_(A3Vec*);

/// Reserve space for (at least) `additional` extra elements.
A3_EXPORT void a3_vec_reserve(A3Vec*, size_t additional);

/// Destroy a vector, freeing the backing storage.
A3_EXPORT void a3_vec_destroy(A3Vec*);

/// Get a pointer to the element at the given index, or `NULL` if no such element exists.
A3_EXPORT void* a3_vec_at(A3Vec*, size_t);

/// Get a pointer to the first element in the collection, or `NULL` if empty.
A3_EXPORT void* a3_vec_first(A3Vec*);

A3_H_END

///
///    void A3_VEC_INIT(T, A3Vec*);
///
/// \brief Initialize a ::A3Vec.
#define A3_VEC_INIT(T, V) a3_vec_init_((V), sizeof(T), alignof(T), 8)

///
///    void A3_VEC_INIT(T, A3Vec*, size_t cap);
///
/// \brief Initialize a ::A3Vec with the specified capacity.
#define A3_VEC_INIT_WITH_CAPACITY(T, V, C) a3_vec_init_((V), sizeof(T), alignof(T), 8)

///
///    void A3_VEC_PUSH(A3Vec*, T const*);
///
/// \brief Push an element to the end of the ::A3Vec, growing as required.
#define A3_VEC_PUSH(V, E) memcpy(a3_vec_write_ptr_(V), (void*)(E), (V)->elem_size)

///
///    T A3_VEC_POP(A3Vec*, T);
///
/// \brief Remove the last element and return it.
#define A3_VEC_POP(T, V)                                                                           \
    (assert(sizeof(T) == (V)->elem_size && alignof(T) == (V)->elem_align && (V)->len),             \
     *(T*)a3_vec_pop_(V))

///
///    T* A3_VEC_AT(T, A3Vec*, size_t index);
///
/// \brief Get a pointer to the element at the given index, or `NULL` if no such element exists.
#define A3_VEC_AT(T, V, I) ((T*)a3_vec_at((V), (I)))

///
///    void A3_VEC_CLEAR(A3Vec*);
///
/// \brief Synonym of ::a3_vec_destroy().
#define A3_VEC_CLEAR a3_vec_destroy

/// Iterate over an ::A3Vec.
#define A3_VEC_FOR_EACH(T, NAME, V)                                                                \
    for (T* NAME = (T*)a3_vec_first(V); (NAME) < (T*)(V)->buf + (V)->len; (NAME)++)
