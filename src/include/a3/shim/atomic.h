/*
 * ATOMIC SHIM -- Cross-platform shim for stdatomic.
 *
 * Copyright (c) 2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 *
 * Some platforms do not provide stdatomic. For now, this shim provides only the primitives used by
 * A3Spmc.
 */

#pragma once

#if defined(__STDC__) && __STDC_VERSION__ >= 201100L && !defined(__STDC_NO_ATOMICS__)

#include <stdatomic.h>

typedef enum A3MemoryOrder {
    A3_RELAXED = memory_order_relaxed,
    A3_ACQUIRE = memory_order_acquire,
    A3_RELEASE = memory_order_release,
    A3_ACQ_REL = memory_order_acq_rel,
    A3_SEQ_CST = memory_order_seq_cst,
} A3MemoryOrder;

#define A3_ATOMIC(TY) TY _Atomic

#define A3_ATOMIC_INIT                  atomic_init
#define A3_ATOMIC_LOAD                  atomic_load_explicit
#define A3_ATOMIC_STORE                 atomic_store_explicit
#define A3_ATOMIC_EXCHANGE              atomic_exchange_explicit
#define A3_ATOMIC_COMPARE_EXCHANGE      atomic_compare_exchange_strong_explicit
#define A3_ATOMIC_COMPARE_EXCHANGE_WEAK atomic_compare_exchange_weak_explicit
#define A3_ATOMIC_FETCH_ADD             atomic_fetch_add_explicit

#else

#include <stdbool.h>
#include <stddef.h>

#include <a3/macro.h>
#include <a3/types.h>

typedef enum A3MemoryOrder {
    A3_RELAXED = 0,
    A3_ACQUIRE = 1,
    A3_RELEASE = 2,
    A3_ACQ_REL = A3_ACQUIRE | A3_RELEASE,
    A3_SEQ_CST = 4,
} A3MemoryOrder;

#define A3_ATOMIC(TY) TY

#define A3_ATOMIC_INIT(ATOM, VALUE)                                                                \
    A3_M_BEGIN                                                                                     \
        *(ATOM) = (VALUE);                                                                         \
    A3_M_END

#define _A3_ATOMIC_DISPATCH(ATOM, METHOD, ...)                                                     \
    _Generic((*ATOM), size_t : a3_atomic_usize_##METHOD, void* : a3_atomic_ptr_##METHOD)(__VA_ARGS__)

#define A3_ATOMIC_LOAD(ATOM, ORDER) _A3_ATOMIC_DISPATCH(ATOM, load, (ATOM), (ORDER))
#define A3_ATOMIC_STORE(ATOM, VALUE, ORDER)                                                        \
    _A3_ATOMIC_DISPATCH(ATOM, store, (ATOM), (VALUE), (ORDER))
#define A3_ATOMIC_EXCHANGE(ATOM, VALUE, ORDER)                                                     \
    _A3_ATOMIC_DISPATCH(ATOM, exchange, (ATOM), (VALUE), (ORDER))
#define A3_ATOMIC_COMPARE_EXCHANGE(ATOM, EXPECTED, VALUE, ORDER, ORDER_FAIL)                       \
    _A3_ATOMIC_DISPATCH(ATOM, compare_exchange, (ATOM), (EXPECTED), (VALUE), (ORDER), (ORDER_FAIL))
#define A3_ATOMIC_COMPARE_EXCHANGE_WEAK(ATOM, EXPECTED, VALUE, ORDER, ORDER_FAIL)                  \
    _A3_ATOMIC_DISPATCH(ATOM, compare_exchange_weak, (ATOM), (EXPECTED), (VALUE), (ORDER),         \
                        (ORDER_FAIL))
#define A3_ATOMIC_FETCH_ADD(ATOM, RHS, ORDER)                                                      \
    _A3_ATOMIC_DISPATCH(ATOM, fetch_add, (ATOM), (RHS), (ORDER))

A3_EXPORT void*  a3_atomic_ptr_load(A3_ATOMIC(void*) const*, A3MemoryOrder);
A3_EXPORT size_t a3_atomic_usize_load(A3_ATOMIC(size_t) const*, A3MemoryOrder);
A3_EXPORT void   a3_atomic_ptr_store(A3_ATOMIC(void*) *, void*, A3MemoryOrder);
A3_EXPORT void   a3_atomic_usize_store(A3_ATOMIC(size_t) *, size_t, A3MemoryOrder);
A3_EXPORT void*  a3_atomic_ptr_exchange(A3_ATOMIC(void*) *, void*, A3MemoryOrder);
A3_EXPORT size_t a3_atomic_usize_exchange(A3_ATOMIC(size_t) *, size_t, A3MemoryOrder);
A3_EXPORT bool   a3_atomic_ptr_compare_exchange(A3_ATOMIC(void*) *, void** expected, void* value,
                                                A3MemoryOrder order, A3MemoryOrder order_fail);
A3_EXPORT bool a3_atomic_usize_compare_exchange(A3_ATOMIC(size_t) *, size_t* expected, size_t value,
                                                A3MemoryOrder order, A3MemoryOrder order_fail);
A3_EXPORT bool a3_atomic_ptr_compare_exchange_weak(A3_ATOMIC(void*) *, void** expected, void* value,
                                                   A3MemoryOrder order, A3MemoryOrder order_fail);
A3_EXPORT bool a3_atomic_usize_compare_exchange_weak(A3_ATOMIC(size_t) *, size_t* expected,
                                                     size_t value, A3MemoryOrder order,
                                                     A3MemoryOrder order_fail);
A3_EXPORT void*  a3_atomic_ptr_fetch_add(A3_ATOMIC(void*) *, ptrdiff_t rhs, A3MemoryOrder);
A3_EXPORT size_t a3_atomic_usize_fetch_add(A3_ATOMIC(size_t) *, size_t rhs, A3MemoryOrder);

#endif
