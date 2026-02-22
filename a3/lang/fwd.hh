/*
 * FWD — Nicer wrapper for std::forward
 *
 * Copyright (c) 2023, 2025, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

#pragma once

#define A3_FWD(A) static_cast<decltype(A)&&>(A)
