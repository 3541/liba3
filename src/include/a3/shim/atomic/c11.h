/*
 * ATOMIC SHIM -- Cross-platform shim for stdatomic.
 *
 * Copyright (c) 2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 *
 * The simple case — just forward to stdatomic.
 */

#pragma once

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
