/*
 * SPMC -- A finite-size thread-safe single-producer multiple-consumer queue.
 *
 * Copyright (c) 2022, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#include <a3/shim/atomic.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#include <a3/spmc.h>
#include <a3/util.h>

void a3_spmc_init(A3Spmc* q, size_t cap) {
    assert(q);

    q->cap = 1;
    while (q->cap < cap)
        q->cap <<= 1;
    q->mask = q->cap - 1;

    A3_UNWRAPN(q->data, calloc(q->cap, sizeof(*q->data)));
    A3_ATOMIC_INIT(&q->head, 0);
    A3_ATOMIC_INIT(&q->end, 0);
}

A3Spmc* a3_spmc_new(size_t cap) {
    A3_UNWRAPNI(A3Spmc*, ret, calloc(1, sizeof(*ret)));
    a3_spmc_init(ret, cap);

    return ret;
}

void a3_spmc_destroy(A3Spmc* q) {
    assert(q);
    assert(q->head == q->end);

    free(q->data);
}

void a3_spmc_free(A3Spmc* q) {
    a3_spmc_destroy(q);
    free(q);
}

static size_t a3_spmc_index(A3Spmc* q, size_t ticket) {
    assert(q);

    return ticket & q->mask;
}

void* a3_spmc_try_dequeue(A3Spmc* q) {
    assert(q);

    size_t end  = A3_ATOMIC_LOAD(&q->end, A3_ACQUIRE);
    size_t head = A3_ATOMIC_LOAD(&q->head, A3_ACQUIRE);

    for (size_t i = 0; i < 5; i++) {
        if (head >= end)
            return NULL;

        if (A3_ATOMIC_COMPARE_EXCHANGE(&q->head, &head, head + 1, A3_ACQ_REL, A3_ACQUIRE))
            goto found;
    }
    return NULL;

found:
    // TODO: seq_cst?
    return A3_ATOMIC_EXCHANGE(&q->data[a3_spmc_index(q, head)], NULL, A3_ACQ_REL);
}

void* a3_spmc_dequeue(A3Spmc* q) {
    assert(q);

    size_t head = A3_ATOMIC_FETCH_ADD(&q->head, 1, A3_ACQ_REL);

    // TODO: Smarter ways to block.
    while (head >= A3_ATOMIC_LOAD(&q->end, A3_ACQUIRE))
        ;

    return A3_ATOMIC_EXCHANGE(&q->data[a3_spmc_index(q, head)], NULL, A3_ACQ_REL);
}

bool a3_spmc_try_enqueue(A3Spmc* q, void* elem) {
    assert(q);

    size_t end  = A3_ATOMIC_LOAD(&q->end, A3_RELAXED);
    size_t head = A3_ATOMIC_LOAD(&q->head, A3_ACQUIRE);
    if (end >= head + q->cap)
        return false;

    size_t end_index = a3_spmc_index(q, end);
    if (A3_ATOMIC_LOAD(&q->data[end_index], A3_ACQUIRE))
        return false;

    A3_ATOMIC_STORE(&q->data[end_index], elem, A3_RELEASE);
    A3_ATOMIC_FETCH_ADD(&q->end, 1, A3_RELEASE);

    return true;
}

void a3_spmc_enqueue(A3Spmc* q, void* elem) {
    assert(q);

    size_t end = A3_ATOMIC_LOAD(&q->end, A3_RELAXED);
    while (end >= A3_ATOMIC_LOAD(&q->head, A3_ACQUIRE) + q->cap)
        ;

    size_t end_index = a3_spmc_index(q, end);
    while (A3_ATOMIC_LOAD(&q->data[end_index], A3_ACQUIRE))
        ;

    A3_ATOMIC_STORE(&q->data[end_index], elem, A3_RELEASE);
    A3_ATOMIC_FETCH_ADD(&q->end, 1, A3_RELEASE);
}
