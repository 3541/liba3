/*
 * MACRO -- Macro-writing utilities.
 *
 * Copyright (c) 2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

/// A hack for MSVC's broken preprocessor, which expands __VA_ARGS__ incorrectly.
#define A3_M_MSVC_VA_HACK(E) E
/// Expands to its fourth argument.
#define A3_M_ARG4(A1, A2, A3, A4, ...) A4
