/*
 * DECL -- Decorators and declarations.
 *
 * Copyright (c) 2020, Alex O'Brien
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#define ALLOW_UNUSED  __attribute__((unused))
#define ALWAYS_INLINE inline __attribute__((always_inline))
#define EXPORT        __attribute__((__visibility__("default")))

#define THREAD_LOCAL _Thread_local
