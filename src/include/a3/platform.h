/*
 * PLATFORM -- Cross-platform shims for annoying little differences. A variant
 * which exposes nicer, but likelier to collide names is available in
 * platform_private.h.
 *
 * Copyright (c) 2020, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

// Types
#ifndef a3_HAVE_ssize_t
#ifdef _MSC_VER
#include <basetsd.h>
typedef SSIZE_T A3_SSIZE_T;
#else
#warning "ssize_t not found and no known alternative."
#endif
#else
#include <sys/types.h>
typedef ssize_t A3_SSIZE_T;
#endif

// Renamings
#ifdef _MSC_VER
#define strncasecmp _strnicmp
#endif

// Attributes
#ifdef _WIN32
// Note: Yes, this works on mingw GCC.
#define EXPORT __declspec(dllexport)
#elif defined(__GNUC__) || defined(__clang__)
#define EXPORT        __attribute__((__visibility__("default")))
#else // !_WIN32 && (__GNUC__ || __clang__)
#define EXPORT
#endif // !_WIN32 && !__GNUC__ && !__clang__

#ifdef _MSC_VER
#define ALLOW_UNUSED
#define ALWAYS_INLINE __forceinline
#else
#if !defined(__GNUC__) && !defined(__clang__)
#warning                                                                       \
    "Unknown compiler variant for attributes. Defaulting to GCC/Clang-style."
#endif
#define ALLOW_UNUSED  __attribute__((unused))
#define ALWAYS_INLINE inline __attribute__((always_inline))
#endif

#ifdef a3_HAVE__Thread_local
#define THREAD_LOCAL _Thread_local
#else
#ifdef _MSC_VER
#define THREAD_LOCAL __declspec(thread)
#else
#warning "Missing _Thread_local with no known alternative."
#endif
#endif
