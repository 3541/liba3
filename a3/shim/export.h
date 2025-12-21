/*
 * EXPORT
 *
 * Copyright (c) 2025, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

 #pragma once

 #include "a3/shim/platform.h"

#ifdef A3_PLATFORM_OS_WINDOWS
#define A3_EXPORT __declspec(dllexport)
#elif defined(A3_PLATFORM_COMPILER_GCC_LIKE)
#define A3_EXPORT __attribute__((__visibility__("default")))
#else
#define A3_EXPORT
#endif
 
