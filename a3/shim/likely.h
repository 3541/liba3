/*
 * LIKELY — Cross-platform shim for likely/unlikely attributes.
 *
 * Copyright (c) 2020, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

#pragma once

#include "a3/shim/platform.h"
#include "a3/shim/attribute.h"

#ifdef A3_PLATFORM_COMPILER_GCC_LIKE
#define A3_LIKELY(E)   (__builtin_expect(!!(E), 1))
#define A3_UNLIKELY(E) (__builtin_expect(!!(E), 0))
#elif A3_HAS_CPP_ATTR(likely) && A3_HAS_CPP_ATTR(unlikely)
#define A3_LIKELY(E)   (E) [[likely]]
#define A3_UNLIKELY(E) (E) [[unlikely]]
#else
#define A3_LIKELY(E)   (E)
#define A3_UNLIKELY(E) (E)
#endif
