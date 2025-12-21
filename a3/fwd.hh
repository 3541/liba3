/*
 * FWD -- Nicer wrapper for std::forward
 *
 * Copyright (c) 2023, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

#pragma once

#include <utility>

#define A3_FWD(A) std::forward<decltype(A)>(A)
