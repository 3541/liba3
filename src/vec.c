/*
 * VEC -- A type-generic vector.
 *
 * Copyright (c) 2020-2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#include <a3/shim/aligned_alloc.h>
#include <stddef.h>
#include <stdint.h>

#include <a3/util.h>
#include <a3/vec.h>

static inline size_t align_down(size_t v, size_t align) { return v & ~(align - 1); }
static inline size_t align_up(size_t v, size_t align) { return align_down(v + align - 1, align); }

static void* a3_vec_index(A3Vec* vec, size_t index) {
    assert(vec);

    if (!vec->buf || index > vec->cap)
        return NULL;

    return (uint8_t*)vec->buf + index * vec->elem_size;
}

void a3_vec_init_(A3Vec* vec, size_t elem_size, size_t elem_align, size_t cap) {
    assert(vec);
    assert(elem_size > 0);
    assert(elem_align > 0);

    *vec = (A3Vec) { .cap        = cap,
                     .len        = 0,
                     .elem_size  = align_up(elem_size, elem_align),
                     .elem_align = elem_align,
                     .buf        = NULL };
    if (cap)
        A3_UNWRAPN(vec->buf, a3_shim_aligned_alloc(cap * elem_size, elem_align));
}

void* a3_vec_write_ptr_(A3Vec* vec) {
    assert(vec);

    a3_vec_reserve(vec, 1);
    return a3_vec_index(vec, vec->len++);
}

void* a3_vec_pop_(A3Vec* vec) {
    assert(vec);

    if (!vec->buf || !vec->len)
        return NULL;

    return a3_vec_index(vec, --vec->len);
}

void a3_vec_reserve(A3Vec* vec, size_t additional) {
    assert(vec);

    if (vec->len + additional <= vec->cap)
        return;

    if (!vec->buf) {
        vec->cap = additional;
        A3_UNWRAPN(vec->buf, a3_shim_aligned_alloc(vec->cap * vec->elem_size, vec->elem_align));
        return;
    }

    while (vec->cap - vec->len < additional)
        vec->cap *= 2;

    A3_UNWRAPNI(void*, new_buf, a3_shim_aligned_alloc(vec->cap * vec->elem_size, vec->elem_align));
    memcpy(new_buf, vec->buf, vec->len * vec->elem_size);
    a3_shim_aligned_free(vec->buf);
    vec->buf = new_buf;
}

void a3_vec_destroy(A3Vec* vec) {
    assert(vec);

    if (vec->buf)
        a3_shim_aligned_free(vec->buf);
    vec->len = 0;
    vec->cap = 0;
}

void* a3_vec_at(A3Vec* vec, size_t index) {
    assert(vec);

    if (index >= vec->len)
        return NULL;

    return (uint8_t*)vec->buf + index * vec->elem_size;
}

void* a3_vec_first(A3Vec* vec) {
    assert(vec);

    if (!vec->buf)
        return NULL;

    return vec->buf;
}
