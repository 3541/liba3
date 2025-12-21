/*
 * C/C++ SHIM — Macros for header compatibility across C/C++.
 *
 * Copyright (c) 2025, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

#pragma once

#ifdef __cplusplus
#define A3_H_BEGIN   extern "C" {
#define A3_H_END     }
#define A3_CONSTEXPR constexpr
#define A3_EMPTY_INIT                                                                              \
    {}
#else
#define A3_H_BEGIN
#define A3_H_END
#define A3_CONSTEXPR
#define A3_EMPTY_INIT
#endif
