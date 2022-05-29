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

#if defined(__STDC__) && __STDC_VERSION__ >= 201100L && !defined(__STDC_NO_ATOMICS__) &&           \
    (defined(__llvm__) || !defined(__GNUC__) ||                                                    \
     (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 9)))
#include <a3/shim/atomic/c11.h>
#elif defined(__ATOMIC_RELAXED)
#include <a3/shim/atomic/gcc.h>
#else
#include <a3/shim/atomic/other.h>
#endif
