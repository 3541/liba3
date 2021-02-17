/*
 * PLATFORM UTILITIES -- Platform-specific utility functions.
 *
 * Copyright (c) 2020, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#include <string.h>

#include <a3/util.h>

#ifdef a3_HAVE_memset_s
#define A3_STRUCT_ZERO(S) A3_UNWRAPND(!memset_s((S), sizeof(*(S)), 0, sizeof(*(S))))
#elif defined(a3_HAVE_explicit_bzero)
#define A3_STRUCT_ZERO(S) explicit_bzero((S), sizeof(*(S)))
#elif defined(a3_HAVE_SecureZeroMemory)
#include <Windows.h>
#define A3_STRUCT_ZERO(S) SecureZeroMemory((S), sizeof(*(S)))
#else
#ifndef _MSC_VER
#warning "Couldn't find a `memset_s` alternative. `A3_STRUCT_ZERO` may be optimized out."
#endif
#define A3_STRUCT_ZERO(S) memset((S), 0, sizeof(*(S)))
#endif
