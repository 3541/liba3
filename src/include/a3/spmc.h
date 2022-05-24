/*
 * SPMC -- A finite-size thread-safe single-producer multiple-consumer queue.
 *
 * Copyright (c) 2022, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <stdatomic.h>
#include <stdbool.h>

#define A3_ATOMIC_PTR(TY) TY* _Atomic

/// An SPMC queue. See ::a3_spmc_init.
typedef struct A3Spmc {
    A3_ATOMIC_PTR(void) * data; //< The actual backing array.
    size_t cap;                 //< The queue's capacity.
    size_t mask;

    atomic_size_t head; //< The index of the oldest element in the queue.
    atomic_size_t end;  //< The index of the next free slot. If head == end, queue is empty.

    // TODO: Assert that producer functions are called from the right thread in debug mode.
} A3Spmc;

/// Initialize a queue. Capacity will be aligned up to the nearest power of two. When debug
/// assertions are enabled, it is assumed that this is called by the producer. If not, manually
/// override the producer field.
void a3_spmc_init(A3Spmc*, size_t cap);

/// Allocate and initialize a new queue.
A3Spmc* a3_spmc_new(size_t cap);

/// Destroy a queue. Frees the backing storage, but does nothing to the contents. NOT THREAD SAFE.
/// Ensure all consumers are gone before calling.
void a3_spmc_destroy(A3Spmc*);

/// Free and destroy an allocated queue.
void a3_spmc_free(A3Spmc*);

/// Try to remove the first element from the queue, if any. May fail spuriously.
void* a3_spmc_try_dequeue(A3Spmc*);

/// Block until an element is available, then return it.
void* a3_spmc_dequeue(A3Spmc*);

/// Enqueue an element. Returns `true` if successful, `false` if not. NOT THREAD SAFE. Call only
/// from the consumer thread.
bool a3_spmc_try_enqueue(A3Spmc*, void* elem);

/// Enqueue an element. Blocks until complete. NOT THREAD SAFE. Call only from the consumer thread.
void a3_spmc_enqueue(A3Spmc*, void* elem);
