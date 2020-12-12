/*
 * HASH TABLE -- A type-generic hash table. Uses open addressing with Robin Hood
 * hashing. Uses HighwayHash for the default hash function.
 *
 * Copyright (c) 2020, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// From highwayhash.h. Forward-declared since the library header causes unused
// function warnings.
uint64_t HighwayHash64(const uint8_t* data, size_t size, const uint64_t key[4]);

#include <a3/cpp.h>

#define HT_LOAD_FACTOR 90
#define HT_TOMBSTONE   (1ULL << 63)

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
        size_t   size;                                                         \
        size_t   cap;                                                          \
        uint64_t hash_key[4];                                                  \
        HT_ENTRY(K, V) * entries;                                              \
    };                                                                         \
                                                                               \
    H_END

#define HT_HASH(K, V)         K##V##_ht_hash
#define HT_DEFAULT_HASH(K, V) K##V##_ht_default_hash
#define HT_INSERT_AT(K, V)    K##V##_ht_insert_at

// Define methods with a custom hash function. H has the signature:
//     uint64_t H(HT(K, V)* table, K* key);
#define HT_DEFINE_METHODS_HASHER(K, V, H)                                      \
    static uint64_t HT_HASH(K, V)(HT(K, V) * table, K key) {                   \
        assert(table);                                                         \
        uint64_t ret = H(table, key) & ~HT_TOMBSTONE;                          \
        return ret ? ret : 1;                                                  \
    }                                                                          \
                                                                               \
    static void HT_INSERT_AT(K, V)(HT(K, V) * table, uint64_t hash, K key,     \
                                   V value) {                                  \
        assert(table);                                                         \
        for (size_t i = hash % table->cap;; i = (i + 1) % table->cap) { \
            HT_ENTRY(K, V)* current_entry = &table->entries[i]; \
            if (!current_entry.hash) { \
                current_entry.key = key; \
                current_entry.value = value; \
                current_entry.hash = hash; \
            } \
        } \
    }

// Define methods with HighwayHash as the hash function. Helpers have the
// signatures:
//     const uint8_t* KEY_BYTES(K* key);
//           size_t   KEY_SIZE(K* key);
#define HT_DEFINE_METHODS(K, V, KEY_BYTES, KEY_SIZE)                           \
    static uint64_t HT_DEFAULT_HASH(K, V)(HT(K, V) * table, K key) {           \
        assert(table);                                                         \
        return HighwayHash64(KEY_BYTES(key), KEY_SIZE(key), table->hash_key);  \
    }                                                                          \
                                                                               \
    HT_DEFINE_METHODS_HASHER(K, V, HT_DEFAULT_HASH(K, V))
