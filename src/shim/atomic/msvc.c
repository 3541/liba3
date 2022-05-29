/*
 * ATOMIC SHIM -- Cross-platform shim for stdatomic.
 *
 * Copyright (c) 2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 *
 * Windows does not provide stdatomic. For now, this shim provides only the primitives used by
 * A3Spmc. It is probably possible to do a more intelligent mapping of memory orders onto variants
 * of the Windows Interlocked functions, but for now, this is definitely /correct/.
 */

#include <Windows.h>
#include <a3/shim/atomic.h>
#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>

void* a3_atomic_ptr_load(A3_ATOMIC(void*) const* atom, A3MemoryOrder order) {
    (void)order;

    MemoryBarrier();
    return *atom;
}

size_t a3_atomic_usize_load(A3_ATOMIC(size_t) const* atom, A3MemoryOrder order) {
    (void)order;

    MemoryBarrier();
    return *atom;
}

void a3_atomic_ptr_store(A3_ATOMIC(void*) * atom, void* value, A3MemoryOrder order) {
    (void)order;

    *atom = value;
    MemoryBarrier();
}

void a3_atomic_usize_store(A3_ATOMIC(size_t) * atom, size_t value, A3MemoryOrder order) {
    (void)order;

    *atom = value;
    MemoryBarrier();
}

void* a3_atomic_ptr_exchange(A3_ATOMIC(void*) * atom, void* value, A3MemoryOrder order) {
    (void)order;

    return InterlockedExchangePointer(atom, value);
}

size_t a3_atomic_usize_exchange(A3_ATOMIC(size_t) * atom, size_t value, A3MemoryOrder order) {
    (void)order;

#ifdef _WIN64
    return InterlockedExchange64((LONG64*)atom, value);
#else
    return InterlockedExchange((LONG*)atom, value);
#endif
}

bool a3_atomic_ptr_compare_exchange(A3_ATOMIC(void*) * atom, void** expected, void* value,
                                    A3MemoryOrder order, A3MemoryOrder order_fail) {
    (void)order;

    if (InterlockedCompareExchangePointer(atom, value, *expected) == *expected)
        return true;

    *expected = A3_ATOMIC_LOAD(atom, order_fail);
    return false;
}

bool a3_atomic_usize_compare_exchange(A3_ATOMIC(size_t) * atom, size_t* expected, size_t value,
                                      A3MemoryOrder order, A3MemoryOrder order_fail) {
    (void)order;

#ifdef _WIN64
    if ((size_t)InterlockedCompareExchange64((LONG64*)atom, value, *expected) == *expected)
        return true;
#else
    if (InterlockedCompareExchange((LONG*)atom, value, *expected) == *expected)
        return true;
#endif

    *expected = A3_ATOMIC_LOAD(atom, order_fail);
    return false;
}

bool a3_atomic_ptr_compare_exchange_weak(A3_ATOMIC(void*) * atom, void** expected, void* value,
                                         A3MemoryOrder order, A3MemoryOrder order_fail) {
    return A3_ATOMIC_COMPARE_EXCHANGE(atom, expected, value, order, order_fail);
}

bool a3_atomic_usize_compare_exchange_weak(A3_ATOMIC(size_t) * atom, size_t* expected, size_t value,
                                           A3MemoryOrder order, A3MemoryOrder order_fail) {
    return A3_ATOMIC_COMPARE_EXCHANGE(atom, expected, value, order, order_fail);
}

size_t a3_atomic_usize_fetch_add(A3_ATOMIC(size_t) * atom, size_t rhs, A3MemoryOrder order) {
    (void)order;

#ifdef _WIN64
    size_t ret = InterlockedExchangeAdd64((LONG64*)atom, rhs);
    assert(ret < INT64_MAX);
#else
    size_t ret = InterlockedExchangeAdd((LONG*)atom, rhs);
    assert(ret < LONG_MAX);
#endif

    return ret;
}
