/*
 * PLATFORM TYPES -- Cross-platform shims for annoying little differences.
 *
 * Copyright (c) 2020-2021, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

// Types
#ifdef _MSC_VER
#include <basetsd.h>
typedef SSIZE_T A3_SSIZE_T;
#define A3_NO_SSIZE_T
#else
#include <sys/types.h>
typedef ssize_t A3_SSIZE_T;
#endif

// Attributes
#ifdef _WIN32
// Note: Yes, this works on mingw GCC.
#define A3_EXPORT __declspec(dllexport)
#elif defined(__GNUC__) || defined(__clang__)
#define A3_EXPORT __attribute__((__visibility__("default")))
#else // !_WIN32 && (__GNUC__ || __clang__)
#define A3_EXPORT
#endif // !_WIN32 && !__GNUC__ && !__clang__

#ifdef _MSC_VER
#define A3_ALLOW_UNUSED
#define A3_ALWAYS_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define A3_ALLOW_UNUSED  __attribute__((unused))
#define A3_ALWAYS_INLINE static inline __attribute__((always_inline)) A3_ALLOW_UNUSED
#else // !_MSC_VER && (__GNUC__ || __clang__)
#define A3_ALLOW_UNUSED
#define A3_ALWAYS_INLINE
#endif

#ifdef __cplusplus
#define A3_THREAD_LOCAL thread_local
#elif __STDC_VERSION__ >= 201112L
#define A3_THREAD_LOCAL _Thread_local
#elif defined(_MSC_VER)
#define A3_THREAD_LOCAL __declspec(thread)
#else
#define A3_THREAD_LOCAL __thread
#endif
