/*
 * CONTAINER OF — Cross-platform type declarations.
 *
 * Copyright (c) 2025, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

#pragma once

#include <stdint.h>

#define A3_CONTAINER_OF(PTR, TY, FIELD) ((TY*)((uintptr_t)(void*)(PTR) - offsetof(TY, FIELD)))
