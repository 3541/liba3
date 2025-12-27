/*
 * ATTRIBUTES SHIM — Miscellaneous attributes.
 *
 * Copyright (c) 2025, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

#pragma once

#include "a3/shim/attribute.h"
#include "a3/shim/platform.h"

#ifdef __cplusplus
#if A3_HAS_CPP_ATTR(gnu::always_inline)
#define A3_ALWAYS_INLINE [[gnu::always_inline]] inline
#endif
#endif

#if !defined(A3_ALWAYS_INLINE)
#if A3_HAS_ATTR(__always_inline__) || defined(A3_PLATFORM_COMPILER_GCC_LIKE)
#define A3_ALWAYS_INLINE __attribute__((__always_inline__)) static inline
#else
#define A3_ALWAYS_INLINE static inline
#endif
#endif
