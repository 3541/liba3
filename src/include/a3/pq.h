/*
 * PRIORITY QUEUE -- A type-generic priority queue backed by a binary heap.
 *
 * Copyright (c) 2020-2021, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 *
 * The priority queue implemented here is as yet untested, and should be considered a sketch, rather
 * than an actual implementation.
 */

#pragma once

#include <assert.h>
#include <stdlib.h>

#include <a3/cpp.h>
#include <a3/util.h>

#define A3_PQ(TY) struct TY##A3PQ

#define A3_PQ_IMPL_STRUCT(TY)                                                                      \
    A3_H_BEGIN                                                                                     \
    A3_PQ(TY) {                                                                                    \
        TY*    data;                                                                               \
        size_t len;                                                                                \
        size_t cap;                                                                                \
        size_t max_cap;                                                                            \
    };                                                                                             \
    A3_H_END

#define A3_PQ_INIT(TY)    TY##_a3_pq_init
#define A3_PQ_PEEK(TY)    TY##_a3_pq_peek
#define A3_PQ_GET(TY)     TY##_a3_pq_get
#define A3_PQ_SET(TY)     TY##_a3_pq_set
#define A3_PQ_ENQUEUE(TY) TY##_a3_pq_enqueue
#define A3_PQ_HEAPIFY(TY) TY##_a3_pq_heapify
#define A3_PQ_DEQUEUE(TY) TY##_a3_pq_dequeue

A3_H_BEGIN
inline size_t a3_pq_parent(size_t i) { return i / 2; }
inline size_t a3_pq_left_child(size_t i) { return i * 2; }
inline size_t a3_pq_right_child(size_t i) { return i * 2 + 1; }
A3_H_END

#define A3_PQ_DECLARE_METHODS(TY)                                                                  \
    A3_H_BEGIN                                                                                     \
    void A3_PQ_INIT(TY)(A3_PQ(TY)*, size_t initial_cap, size_t max_cap);                           \
    TY*  A3_PQ_PEEK(TY)(A3_PQ(TY)*);                                                               \
    void A3_PQ_ENQUEUE(TY)(A3_PQ(TY)*, TY);                                                        \
    void A3_PQ_HEAPIFY(TY)(A3_PQ(TY)*, size_t i);                                                  \
    TY   A3_PQ_DEQUEUE(TY)(A3_PQ(TY)*);                                                            \
    A3_H_END

#define A3_PQ_IMPL_METHODS(TY, C)                                                                  \
    A3_H_BEGIN                                                                                     \
    void A3_PQ_INIT(TY)(A3_PQ(TY) * this, size_t initial_cap, size_t max_cap) {                    \
        assert(!this->data);                                                                       \
        A3_UNWRAPN(this->data, calloc(initial_cap, sizeof(TY)));                                   \
        this->len     = 0;                                                                         \
        this->cap     = initial_cap;                                                               \
        this->max_cap = max_cap;                                                                   \
    }                                                                                              \
                                                                                                   \
    TY* A3_PQ_PEEK(TY)(A3_PQ(TY) * this) {                                                         \
        assert(this);                                                                              \
        if (this->len == 0)                                                                        \
            return NULL;                                                                           \
        return this->data;                                                                         \
    }                                                                                              \
                                                                                                   \
    /* Convenience functions for 1-based indexing. */                                              \
    INLINE TY* A3_PQ_GET(TY)(A3_PQ(TY) * this, size_t i) {                                         \
        assert(this);                                                                              \
        assert(i > 0);                                                                             \
        assert(i - 1 < this->len);                                                                 \
        return &this->data[i - 1];                                                                 \
    }                                                                                              \
                                                                                                   \
    INLINE void A3_PQ_SET(TY)(A3_PQ(TY) * this, size_t i, TY item) {                               \
        assert(this);                                                                              \
        assert(i > 0);                                                                             \
        assert(i - 1 < this->cap);                                                                 \
        this->data[i - 1] = item;                                                                  \
    }                                                                                              \
                                                                                                   \
    void A3_PQ_ENQUEUE(TY)(A3_PQ(TY) * this, TY item) {                                            \
        assert(this);                                                                              \
        assert(this->len + 1 < this->cap);                                                         \
                                                                                                   \
        size_t i = ++this->len;                                                                    \
        while (i > 1 && C(A3_PQ_GET(TY)(this, pq_parent(i)), &item) > 0) {                         \
            A3_PQ_SET(TY)(this, i, *A3_PQ_GET(TY)(this, pq_parent(i)));                            \
            i = pq_parent(i);                                                                      \
        }                                                                                          \
        A3_PQ_SET(TY)(this, i, item);                                                              \
    }                                                                                              \
                                                                                                   \
    void A3_PQ_HEAPIFY(TY)(A3_PQ(TY) * this, size_t i) {                                           \
        assert(this);                                                                              \
                                                                                                   \
        TY item = *A3_PQ_GET(TY)(this, i);                                                         \
        while (i < this->len / 2) {                                                                \
            size_t min_child = pq_left_child(i);                                                   \
            if (min_child < this->len &&                                                           \
                C(A3_PQ_GET(TY)(this, min_child), A3_PQ_GET(TY)(this, min_child + 1)) > 0)         \
                min_child++;                                                                       \
            if (C(&item, A3_PQ_GET(TY)(this, min_child)) <= 0)                                     \
                break;                                                                             \
            A3_PQ_SET(TY)(this, i, *A3_PQ_GET(TY)(this, min_child));                               \
            i = min_child;                                                                         \
        }                                                                                          \
        A3_PQ_SET(TY)(this, i, item);                                                              \
    }                                                                                              \
                                                                                                   \
    TY A3_PQ_DEQUEUE(TY)(A3_PQ(TY) * this) {                                                       \
        assert(this);                                                                              \
        assert(this->len > 0);                                                                     \
        if (this->len == 0)                                                                        \
            A3_PANIC("DEQUEUE on empty A3_PQ.");                                                   \
                                                                                                   \
        TY ret = *A3_PQ_PEEK(TY)(this);                                                            \
        A3_PQ_SET(TY)(this, 1, *A3_PQ_GET(TY)(this, this->len--));                                 \
        A3_PQ_HEAPIFY(TY)(this, 1);                                                                \
                                                                                                   \
        return ret;                                                                                \
    }                                                                                              \
    A3_H_END
