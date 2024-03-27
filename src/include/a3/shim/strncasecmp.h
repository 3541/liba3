/*
 * STRNCASECMP SHIM -- Cross-platform shim for strncasecmp.
 *
 * Copyright (c) 2022, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 *
 * Some platforms provide the same functionality under different names.
 */

#pragma once

#include <stddef.h>

#include <a3/cpp.h>
#include <a3/types.h>

A3_H_BEGIN

A3_EXPORT int a3_shim_strncasecmp(char const*, char const*, size_t n);

A3_H_END
