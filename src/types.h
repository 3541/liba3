/*
 * PLATFORM TYPES (PRIVATE) -- Nice names for platform-specific concepts.
 *
 * Copyright (c) 2020-2021, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <a3/types.h>

// Types
#ifdef A3_NO_SSIZE_T
typedef A3_SSIZE_T ssize_t;
#endif

#define ALLOW_UNUSED  A3_ALLOW_UNUSED
#define ALWAYS_INLINE A3_ALWAYS_INLINE
#define EXPORT        A3_EXPORT
#define THREAD_LOCAL  A3_THREAD_LOCAL
