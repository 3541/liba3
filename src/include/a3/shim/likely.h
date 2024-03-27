/*
 * LIKELY — Cross-platform shim for likely/unlikely attributes.
 *
 * Copyright (c) 2020, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

#pragma once

#if defined(__GNUC__) || defined(__clang__)
#define A3_LIKELY(E)   (__builtin_expect(!!(E), 1))
#define A3_UNLIKELY(E) (__builtin_expect(!!(E), 0))
#elif defined(__has_cpp_attribute)
#if __has_cpp_attribute(likely) && __has_cpp_attribute(unlikely)
#define A3_LIKELY(E)   (E) [[likely]]
#define A3_UNLIKELY(E) (E) [[unlikely]]
#else
#define A3_LIKELY(E)   (E)
#define A3_UNLIKELY(E) (E)
#endif
#else
#define A3_LIKELY(E)   (E)
#define A3_UNLIKELY(E) (E)
#endif
