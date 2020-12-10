/*
 * HASH SET -- A type-generic hash set. Uses open addressing with Robin Hood
 * hashing. Uses SipHash for the default hash function.
 *
 * Copyright (c) 2020, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <stdint.h>

#include <a3/cpp.h>

#define HS_LOAD_FACTOR 90

#define HS(TY)       struct TY##HS
#define HS_ENTRY(TY) struct TY##HSEntry

#define HS_IMPL_STRUCTS(TY)                                                    \
    H_BEGIN                                                                    \
    HS_ENTRY(TY) {                                                             \
        TY       item;                                                         \
        uint64_t hash;                                                         \
    };                                                                         \
                                                                               \
    HS(TY) {                                                                   \
        size_t size;                                                           \
        size_t cap;                                                            \
        HS_ENTRY(TY) * entries;                                                \
    };                                                                         \
    H_END
