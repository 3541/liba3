/*
 * ATTRIBUTE SHIM — Attribute detection.
 *
 * Copyright (c) 2025, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

#pragma once

#ifdef __has_cpp_attribute
#define A3_HAS_CPP_ATTR(A) __has_cpp_attribute
#else
#define A3_HAS_CPP_ATTR(A) 0
#endif
