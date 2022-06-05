/*
 * MACRO -- Macro-writing utilities.
 *
 * Copyright (c) 2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

/// \file macro.h
/// # Macro Utilities
/// Miscellaneous macro utilities and hacks for preprocessor abuse.

#pragma once

/// An abbreviation for the typical do-while macro construct.
#define A3_M_BEGIN do {
/// An abbreviation for the typical do-while macro construct.
#define A3_M_END                                                                                   \
    }                                                                                              \
    while (0)

/// A hack for MSVC's broken preprocessor, which expands `__VA_ARGS__` incorrectly.
#define A3_M_MSVC_VA_HACK(E) E

/// Expands to its first argument.
#define A3_M_ARG1(A1, ...) A1
/// Expands to its second argument.
#define A3_M_ARG2(A1, A2, ...) A2
/// Expands to its third argument.
#define A3_M_ARG3(A1, A2, A3, ...) A3
/// Expands to its fourth argument.
#define A3_M_ARG4(A1, A2, A3, A4, ...) A4

#ifndef DOXYGEN
#define A3_M_PASTE_INNER(A, B) A##B
#endif

/// Paste together two tokens. Useful for line-dependent variable names.
#define A3_M_PASTE(A, B) A3_M_PASTE_INNER(A, B)
