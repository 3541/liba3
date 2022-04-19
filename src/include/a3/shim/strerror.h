/*
 * STRERROR SHIM -- Cross-platform shim for strerror.
 *
 * Copyright (c) 2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 *
 * Versions of strerror_r are mutually incompatible (even on the same platform, depending on
 * _GNU_SOURCE). This shim papers over the differences.
 */

#pragma once

#include <a3/cpp.h>
#include <a3/types.h>

A3_H_BEGIN

A3_EXPORT char* a3_shim_strerror(int errnum, char* buf, size_t len);

A3_H_END
