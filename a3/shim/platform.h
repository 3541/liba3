/*
 * PLATFORM SHIM — Platform definition and detection macros.
 *
 * Copyright (c) 2025, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

#pragma once

#if defined(__unix__) || defined(__unix) || defined(unix) || defined(__APPLE__)
#define A3_PLATFORM_OS_UNIXLIKE
#endif

#ifdef __linux__
#define A3_PLATFORM_OS_LINUX
#endif

#ifdef _WIN32
#define A3_PLATFORM_OS_WINDOWS
#endif

#ifdef __clang__
#define A3_PLATFORM_COMPILER_CLANG
#endif

#ifdef __GNUC__
#define A3_PLATFORM_COMPILER_GCC_LIKE
#endif

#if defined(__GNUC__) && !defined(__clang__)
#define A3_PLATFORM_COMPILER_GCC
#endif

