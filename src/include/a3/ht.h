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
#include <sys/random.h>

#include <a3/cpp.h>

// From highwayhash.h. Forward-declared since the library header causes unused
// function warnings.
H_BEGIN
uint64_t HighwayHash64(const uint8_t* data, size_t size, const uint64_t key[4]);
H_END

#include <a3/cpp.h>

#ifndef HT_INITIAL_CAP
#define HT_INITIAL_CAP 257
#endif

#ifndef HT_LOAD_FACTOR
#define HT_LOAD_FACTOR 90
#endif

#ifndef HT_HASH_KEY_SIZE
#define HT_HASH_KEY_SIZE 4
#endif

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
        size_t   size;                                                         \
        size_t   cap;                                                          \
        uint64_t hash_key[HT_HASH_KEY_SIZE];                                   \
        HT_ENTRY(K, V) * entries;                                              \
    };                                                                         \
                                                                               \
    H_END

#define HT_HASH(K, V)         K##V##_ht_hash
#define HT_DEFAULT_HASH(K, V) K##V##_ht_default_hash
#define HT_PROBE_COUNT(K, V)  K##V##_ht_probe_count
#define HT_INSERT_AT(K, V)    K##V##_ht_insert_at
#define HT_GROW(K, V)         K##V##_ht_grow
#define HT_FIND_ENTRY(K, V)   K##V##_ht_find_entry

#define HT_INIT(K, V)    K##V##_ht_init
#define HT_NEW(K, V)     K##V##_ht_new
#define HT_DESTROY(K, V) K##V##_ht_destroy
#define HT_FREE(K, V)    K##V##_ht_free

#define HT_INSERT(K, V) K##V##_ht_insert
#define HT_FIND(K, V)   K##V##_ht_find

#define HT_DECLARE_METHODS(K, V)                                               \
    H_BEGIN                                                                    \
    void HT_INIT(K, V)(HT(K, V)*);                                             \
    HT(K, V) * HT_NEW(K, V)(void);                                             \
    void HT_DESTROY(K, V)(HT(K, V)*);                                          \
    void HT_FREE(K, V)(HT(K, V)*);                                             \
                                                                               \
    void HT_INSERT(K, V)(HT(K, V)*, K, V);                                     \
    V*   HT_FIND(K, V)(HT(K, V)*, K);                                          \
    H_END

// Define methods with a custom hash function. H has the signature:
//     uint64_t H(HT(K, V)* table, K key);
// C is a comparator, and must return zero for equal keys. It has the signature:
//     int8_t C(K lhs, K rhs);
#define HT_DEFINE_METHODS_HASHER(K, V, H, C)                                     \
    static uint64_t HT_HASH(K, V)(HT(K, V) * table, K key) {                     \
        assert(table);                                                           \
        uint64_t ret = H(table, key) & ~HT_TOMBSTONE;                            \
        return ret ? ret : 1;                                                    \
    }                                                                            \
                                                                                 \
    static size_t HT_PROBE_COUNT(K, V)(HT(K, V) * table, size_t index,           \
                                       uint64_t hash) {                          \
        assert(table);                                                           \
        return (index + table->cap - hash % table->cap) % table->cap;            \
    }                                                                            \
                                                                                 \
    static void HT_INSERT_AT(K, V)(HT(K, V) * table, uint64_t hash, K key,       \
                                   V value) {                                    \
        assert(table);                                                           \
        for (size_t i = hash % table->cap, probe_count = 0;;                     \
             i = (i + 1) % table->cap, probe_count++) {                          \
            HT_ENTRY(K, V)* current_entry = &table->entries[i];                  \
                                                                                 \
            /* Empty hash? It's free real estate. */                             \
            if (!current_entry->hash) {                                          \
                current_entry->key   = key;                                      \
                current_entry->value = value;                                    \
                current_entry->hash  = hash;                                     \
                return;                                                          \
            }                                                                    \
                                                                                 \
            if (HT_PROBE_COUNT(K, V)(table, i, current_entry->hash) <            \
                probe_count) {                                                   \
                if (current_entry->hash & HT_TOMBSTONE) {                        \
                    current_entry->key   = key;                                  \
                    current_entry->value = value;                                \
                    current_entry->hash  = hash;                                 \
                    return;                                                      \
                }                                                                \
                                                                                 \
                HT_ENTRY(K, V) old_entry = *current_entry;                       \
                current_entry->key       = key;                                  \
                current_entry->value     = value;                                \
                current_entry->hash      = hash;                                 \
                key                      = old_entry.key;                        \
                value                    = old_entry.value;                      \
                hash                     = old_entry.hash;                       \
                probe_count              = HT_PROBE_COUNT(K, V)(table, i, hash); \
            }                                                                    \
        }                                                                        \
    }                                                                            \
                                                                                 \
    static void HT_GROW(K, V)(HT(K, V) * table) {                                \
        assert(table);                                                           \
                                                                                 \
        HT_ENTRY(K, V)* prev_entries = table->entries;                           \
        size_t prev_cap              = table->cap;                               \
        table->cap *= 2;                                                         \
        table->entries =                                                         \
            (HT_ENTRY(K, V)*)(calloc(table->cap, sizeof(HT_ENTRY(K, V))));       \
                                                                                 \
        for (size_t i = 0; i < prev_cap; i++) {                                  \
            HT_ENTRY(K, V)* current_entry = &prev_entries[i];                    \
            if (!current_entry->hash || current_entry->hash & HT_TOMBSTONE)      \
                continue;                                                        \
            HT_INSERT_AT(K, V)                                                   \
            (table, current_entry->hash, current_entry->key,                     \
             current_entry->value);                                              \
        }                                                                        \
                                                                                 \
        free(prev_entries);                                                      \
    }                                                                            \
                                                                                 \
    static HT_ENTRY(K, V) * HT_FIND_ENTRY(K, V)(HT(K, V) * table, K key) {       \
        assert(table);                                                           \
                                                                                 \
        uint64_t hash = HT_HASH(K, V)(table, key);                               \
        for (size_t i = hash % table->cap, probe_count = 0;;                     \
             i = (i + 1) % table->cap, probe_count++) {                          \
            HT_ENTRY(K, V)* current_entry = &table->entries[i];                  \
            if (!current_entry->hash || current_entry->hash & HT_TOMBSTONE ||    \
                HT_PROBE_COUNT(K, V)(table, i, current_entry->hash) <            \
                    probe_count)                                                 \
                return NULL;                                                     \
            if (hash == current_entry->hash &&                                   \
                C(key, current_entry->key) == 0)                                 \
                return current_entry;                                            \
        }                                                                        \
    }                                                                            \
                                                                                 \
    void HT_INIT(K, V)(HT(K, V) * table) {                                       \
        assert(table);                                                           \
        table->size   = 0;                                                       \
        table->cap    = HT_INITIAL_CAP;                                          \
        ssize_t bytes = getrandom(table->hash_key, HT_HASH_KEY_SIZE, 0);         \
        assert(bytes == HT_HASH_KEY_SIZE);                                       \
        table->entries =                                                         \
            (HT_ENTRY(K, V)*)calloc(table->cap, sizeof(HT_ENTRY(K, V)));         \
        UNWRAPND(table->entries);                                                \
    }                                                                            \
                                                                                 \
    HT(K, V) * HT_NEW(K, V)() {                                                  \
        HT(K, V)* ret = (HT(K, V)*)calloc(1, sizeof(HT(K, V)));                  \
        HT_INIT(K, V)(ret);                                                      \
        return ret;                                                              \
    }                                                                            \
                                                                                 \
    void HT_DESTROY(K, V)(HT(K, V) * table) {                                    \
        assert(table);                                                           \
        if (table->entries)                                                      \
            free(table->entries);                                                \
    }                                                                            \
                                                                                 \
    void HT_FREE(K, V)(HT(K, V) * table) {                                       \
        assert(table);                                                           \
        HT_DESTROY(K, V)(table);                                                 \
        free(table);                                                             \
    }                                                                            \
                                                                                 \
    void HT_INSERT(K, V)(HT(K, V) * table, K key, V value) {                     \
        assert(table);                                                           \
                                                                                 \
        if (table->size >= table->cap)                                           \
            HT_GROW(K, V)(table);                                                \
                                                                                 \
        table->size++;                                                           \
        HT_INSERT_AT(K, V)(table, HT_HASH(K, V)(table, key), key, value);        \
    }                                                                            \
                                                                                 \
    V* HT_FIND(K, V)(HT(K, V) * table, K key) {                                  \
        assert(table);                                                           \
        HT_ENTRY(K, V)* entry = HT_FIND_ENTRY(K, V)(table, key);                 \
        if (!entry)                                                              \
            return NULL;                                                         \
        return &entry->value;                                                    \
    }

// Define methods with HighwayHash as the hash function. Helpers have the
// signatures:
//     const uint8_t* KEY_BYTES(K* key);
//           size_t   KEY_SIZE(K* key);
// See HT_DEFINE_METHODS_HASHER for information on the comparator C.
#define HT_DEFINE_METHODS(K, V, KEY_BYTES, KEY_SIZE, C)                        \
    static uint64_t HT_DEFAULT_HASH(K, V)(HT(K, V) * table, K key) {           \
        assert(table);                                                         \
        return HighwayHash64(KEY_BYTES(key), KEY_SIZE(key), table->hash_key);  \
    }                                                                          \
                                                                               \
    HT_DEFINE_METHODS_HASHER(K, V, HT_DEFAULT_HASH(K, V), C)
