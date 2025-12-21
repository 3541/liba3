/*
 * ALIGNED_ALLOC SHIM -- Cross-platform shim for aligned_alloc.
 *
 * Copyright (c) 2022, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 *
 * Windows does not provide aligned_alloc.
 */

#pragma once

#include <stddef.h>

#include "a3/shim/cpp.h"
#include "a3/shim/export.h"

A3_H_BEGIN

A3_EXPORT void* a3_shim_aligned_alloc(size_t size, size_t align);
A3_EXPORT void  a3_shim_aligned_free(void*);

A3_H_END
