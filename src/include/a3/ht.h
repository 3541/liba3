/*
 * HASH TABLE -- A type-generic hash table. Uses open addressing with Robin Hood
 * hashing. Uses SipHash for the default hash function.
 *
 * Copyright (c) 2020, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <stdint.h>

#include <highwayhash/c_bindings.h>

#include <a3/cpp.h>

#define HT_LOAD_FACTOR 90
#define HT_TOMBSTONE (1ULL << 63)

#define HT(K, V)       struct K##V##HT
#define HT_ENTRY(K, V) struct K##V##HTEntry

#define HT_DEFINE_STRUCTS(K, V)                                                \
    H_BEGIN                                                                    \
                                                                               \
    HT_ENTRY(K, V) {                                                           \
        K        key;                                                          \
        V        value;                                                        \
        uint64_t hash;                                                         \
    };                                                                         \
                                                                               \
    HT(K, V) {                                                                 \
        size_t size;                                                           \
        size_t cap;                                                            \
        HT_ENTRY(K, V) * entries;                                              \
    };                                                                         \
                                                                               \
    H_END

#define HT_HASH(K) K##_ht_hash
#define HT_DEFAULT_HASH(K) K##_ht_default_hash

// Define methods with a custom hash function. H has the signature:
//     uint64_t H(K* key);
#define HT_DEFINE_METHODS_HASHER(K, V, H) \
    static uint64_t HT_HASH(K)(K* key) { \
        assert(key);                     \
        uint64_t ret = H(key) & ~HT_TOMBSTONE;  \
        return ret? ret : 1; \
    }

// Define methods with siphash as the hash function. KEY_SIZE has the signature:
//     size_t KEY_SIZE(K* key);
#define HT_DEFINE_METHODS(K, V, KEY_SIZE)                 \
    static uint64_t HT_DEFAULT_HASH(K)(K* key) { \
        assert(key);                             \
        uint64_t ret = 0; \
    }
