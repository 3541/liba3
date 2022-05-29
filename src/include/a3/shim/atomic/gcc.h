/*
 * ATOMIC SHIM -- Cross-platform shim for stdatomic.
 *
 * Copyright (c) 2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 *
 * Very old versions of GCC do not provide stdatomic. However, they often do provide atomic
 * intrinsics intended for implementing the C++11 atomic library.
 */

#pragma once

#include <stdbool.h>

#include <a3/macro.h>

typedef enum A3MemoryOrder {
    A3_RELAXED = __ATOMIC_RELAXED,
    A3_ACQUIRE = __ATOMIC_ACQUIRE,
    A3_RELEASE = __ATOMIC_RELEASE,
    A3_ACQ_REL = __ATOMIC_ACQ_REL,
    A3_SEQ_CST = __ATOMIC_SEQ_CST,
} A3MemoryOrder;

#define A3_ATOMIC(TY) TY

#define A3_ATOMIC_INIT(ATOM, VALUE)                                                                \
    A3_M_BEGIN                                                                                     \
        *(ATOM) = (VALUE);                                                                         \
    A3_M_END

#define A3_ATOMIC_LOAD(ATOM, ORDER)                                                                \
    ({                                                                                             \
        __typeof__(*(ATOM)) _ret;                                                                  \
        __atomic_load((ATOM), &_ret, (ORDER));                                                     \
        _ret;                                                                                      \
    })

#define A3_ATOMIC_STORE(ATOM, VALUE, ORDER)                                                        \
    A3_M_BEGIN                                                                                     \
        __typeof__(VALUE) _value = (VALUE);                                                        \
        __atomic_store((ATOM), &_value, (ORDER));                                                  \
    A3_M_END

#define A3_ATOMIC_EXCHANGE(ATOM, VALUE, ORDER)                                                     \
    ({                                                                                             \
        __typeof__(VALUE) _value = (VALUE);                                                        \
        __typeof__(VALUE) _ret;                                                                    \
        __atomic_exchange((ATOM), &_value, &_ret, (ORDER));                                        \
        _ret;                                                                                      \
    })

#define A3_ATOMIC_COMPARE_EXCHANGE(ATOM, EXPECTED, VALUE, ORDER, ORDER_FAIL)                       \
    ({                                                                                             \
        __typeof__(VALUE) _value = (VALUE);                                                        \
        __atomic_compare_exchange((ATOM), (EXPECTED), &_value, /* weak */ false, (ORDER),          \
                                  (ORDER_FAIL));                                                   \
    })

#define A3_ATOMIC_COMPARE_EXCHANGE_WEAK(ATOM, EXPECTED, VALUE, ORDER, ORDER_FAIL)                  \
    ({                                                                                             \
        __typeof__(VALUE) _value = (VALUE);                                                        \
        __atomic_compare_exchange((ATOM), (EXPECTED), &_value, /* weak */ true, (ORDER),           \
                                  (ORDER_FAIL));                                                   \
    })

#define A3_ATOMIC_FETCH_ADD(ATOM, RHS, ORDER) __atomic_fetch_add((ATOM), (RHS), (ORDER))
