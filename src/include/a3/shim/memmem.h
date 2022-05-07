/*
 * MEMMEM SHIM -- Cross-platform shim for memmem.
 *
 * Copyright (c) 2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 *
 * Windows does not provide memmem.
 */

#pragma once

#include <stddef.h>

#include <a3/cpp.h>
#include <a3/types.h>

A3_H_BEGIN

A3_EXPORT void* a3_shim_memmem(void const* haystack, size_t haystack_len, void const* needle,
                               size_t needle_len);

A3_H_END
