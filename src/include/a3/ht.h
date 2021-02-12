/*
 * HASH TABLE -- A type-generic hash table. Uses open addressing with Robin Hood
 * hashing. Uses HighwayHash for the default hash function.
 *
 * Copyright (c) 2020-2021, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

#include <a3/cpp.h>
#include <a3/platform/util.h>
#include <a3/util.h>

// From highwayhash.h. Forward-declared since the library header causes unused
// function warnings.
A3_H_BEGIN
uint64_t HighwayHash64(const uint8_t* data, size_t size, const uint64_t key[4]);
A3_H_END

#ifndef A3_HT_INITIAL_CAP
#define A3_HT_INITIAL_CAP 31ULL
#endif

#ifndef A3_HT_LOAD_FACTOR
#define A3_HT_LOAD_FACTOR 90ULL
#endif

#ifndef A3_HT_HASH_KEY_SIZE
#define A3_HT_HASH_KEY_SIZE 4ULL
#endif

#define A3_HT_ALLOW_GROWTH  true
#define A3_HT_FORBID_GROWTH false

#define A3_HT(K, V)        struct K##V##A3HT
#define A3_HT_ENTRY(K, V)  struct K##V##A3HTEntry
#define A3_HT_DUP_CB(K, V) K##V##A3HTDuplicateCallback

#define A3_HT_DEFINE_STRUCTS(K, V)                                                                 \
    A3_H_BEGIN                                                                                     \
                                                                                                   \
    typedef bool (*A3_HT_DUP_CB(K, V))(V * current_value, V new_value);                            \
                                                                                                   \
    A3_HT_ENTRY(K, V) {                                                                            \
        K        key;                                                                              \
        V        value;                                                                            \
        uint64_t hash;                                                                             \
    };                                                                                             \
                                                                                                   \
    A3_HT(K, V) {                                                                                  \
        size_t   size;                                                                             \
        size_t   cap;                                                                              \
        bool     can_grow;                                                                         \
        uint64_t hash_key[A3_HT_HASH_KEY_SIZE];                                                    \
        A3_HT_DUP_CB(K, V) duplicate_cb;                                                           \
        A3_HT_ENTRY(K, V) * entries;                                                               \
    };                                                                                             \
                                                                                                   \
    A3_H_END

#define A3_HT_HASH(K, V)         K##V##_a3_ht_hash
#define A3_HT_DEFAULT_HASH(K, V) K##V##_a3_ht_default_hash
#define A3_HT_PROBE_COUNT(K, V)  K##V##_a3_ht_probe_count
#define A3_HT_INSERT_AT(K, V)    K##V##_a3_ht_insert_at
#define A3_HT_RESIZE(K, V)       K##V##_a3_ht_resize
#define A3_HT_GROW(K, V)         K##V##_a3_ht_grow
#define A3_HT_FIND_INDEX(K, V)   K##V##_a3_ht_find_index
#define A3_HT_FIND_ENTRY(K, V)   K##V##_a3_ht_find_entry
#define A3_HT_NEXT_ENTRY(K, V)   K##V##a3_ht_next_entry

#define A3_HT_INIT(K, V)             K##V##_a3_ht_init
#define A3_HT_NEW(K, V)              K##V##_a3_ht_new
#define A3_HT_SET_DUPLICATE_CB(K, V) K##V##_a3_ht_set_duplicate_cb
#define A3_HT_DESTROY(K, V)          K##V##_a3_ht_destroy
#define A3_HT_FREE(K, V)             K##V##_a3_ht_free

#define A3_HT_INSERT(K, V)       K##V##_a3_ht_insert
#define A3_HT_FIND(K, V)         K##V##_a3_ht_find
#define A3_HT_DELETE_INDEX(K, V) K##V##_a3_ht_delete_index
#define A3_HT_DELETE(K, V)       K##V##_a3_ht_delete
#define A3_HT_SIZE(K, V)         K##V##_a3_ht_size

#define A3_HT_DECLARE_METHODS(K, V)                                                                \
    A3_H_BEGIN                                                                                     \
    void A3_HT_INIT(K, V)(A3_HT(K, V)*, bool can_grow);                                            \
    A3_HT(K, V) * A3_HT_NEW(K, V)(bool can_grow);                                                  \
    void A3_HT_SET_DUPLICATE_CB(K, V)(A3_HT(K, V)*, A3_HT_DUP_CB(K, V));                           \
    void A3_HT_DESTROY(K, V)(A3_HT(K, V)*);                                                        \
    void A3_HT_FREE(K, V)(A3_HT(K, V)*);                                                           \
                                                                                                   \
    void A3_HT_RESIZE(K, V)(A3_HT(K, V)*, size_t);                                                 \
                                                                                                   \
    bool       A3_HT_INSERT(K, V)(A3_HT(K, V)*, K, V);                                             \
    A3_SSIZE_T A3_HT_FIND_INDEX(K, V)(A3_HT(K, V)*, K);                                            \
    A3_HT_ENTRY(K, V) * A3_HT_FIND_ENTRY(K, V)(A3_HT(K, V)*, K);                                   \
    V*         A3_HT_FIND(K, V)(A3_HT(K, V)*, K);                                                  \
    bool       A3_HT_DELETE_INDEX(K, V)(A3_HT(K, V)*, size_t);                                     \
    bool       A3_HT_DELETE(K, V)(A3_HT(K, V)*, K);                                                \
    A3_SSIZE_T A3_HT_NEXT_ENTRY(K, V)(A3_HT(K, V)*, size_t index);                                 \
                                                                                                   \
    inline size_t A3_HT_SIZE(K, V)(A3_HT(K, V) * table) {                                          \
        assert(table);                                                                             \
        return table->size;                                                                        \
    }                                                                                              \
    A3_H_END

// Define methods with a custom hash function. H has the signature:
//     uint64_t H(A3_HT(K, V)* table, K key);
// C is a comparator, and must return zero for equal keys. It has the signature:
//     int8_t C(K lhs, K rhs);
#define A3_HT_DEFINE_METHODS_HASHER(K, V, H, C)                                                    \
    static uint64_t A3_HT_HASH(K, V)(A3_HT(K, V) * table, K key) {                                 \
        assert(table);                                                                             \
        uint64_t ret = H(table, key);                                                              \
        return ret ? ret : 1;                                                                      \
    }                                                                                              \
                                                                                                   \
    static size_t A3_HT_PROBE_COUNT(K, V)(A3_HT(K, V) * table, size_t index, uint64_t hash) {      \
        assert(table);                                                                             \
        return (index + table->cap - hash % table->cap) % table->cap;                              \
    }                                                                                              \
                                                                                                   \
    static bool A3_HT_INSERT_AT(K, V)(A3_HT(K, V) * table, uint64_t hash, K key, V value) {        \
        assert(table);                                                                             \
        assert(hash);                                                                              \
        assert(table->cap > 0ULL);                                                                 \
                                                                                                   \
        for (size_t i = hash % table->cap, probe_count = 0;;                                       \
             i = (i + 1) % table->cap, probe_count++) {                                            \
            A3_HT_ENTRY(K, V)* current_entry = &table->entries[i];                                 \
                                                                                                   \
            /* Empty hash? It's free real estate. */                                               \
            if (!current_entry->hash) {                                                            \
                current_entry->key   = key;                                                        \
                current_entry->value = value;                                                      \
                current_entry->hash  = hash;                                                       \
                return true;                                                                       \
            }                                                                                      \
                                                                                                   \
            /* Duplicate entry. */                                                                 \
            if (hash == current_entry->hash && C(key, current_entry->key) == 0) {                  \
                if (!table->duplicate_cb)                                                          \
                    return false;                                                                  \
                return table->duplicate_cb(&current_entry->value, value);                          \
            }                                                                                      \
                                                                                                   \
            if (A3_HT_PROBE_COUNT(K, V)(table, i, current_entry->hash) < probe_count) {            \
                A3_HT_ENTRY(K, V) old_entry = *current_entry;                                      \
                current_entry->key          = key;                                                 \
                current_entry->value        = value;                                               \
                current_entry->hash         = hash;                                                \
                key                         = old_entry.key;                                       \
                value                       = old_entry.value;                                     \
                hash                        = old_entry.hash;                                      \
                probe_count                 = A3_HT_PROBE_COUNT(K, V)(table, i, hash);             \
            }                                                                                      \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
    A3_SSIZE_T A3_HT_NEXT_ENTRY(K, V)(A3_HT(K, V) * table, size_t index) {                         \
        for (; index < table->cap; index++)                                                        \
            if (table->entries[index].hash)                                                        \
                return (A3_SSIZE_T)index;                                                          \
        return -1;                                                                                 \
    }                                                                                              \
                                                                                                   \
    void A3_HT_RESIZE(K, V)(A3_HT(K, V) * table, size_t new_cap) {                                 \
        assert(table);                                                                             \
        assert(new_cap > table->cap);                                                              \
                                                                                                   \
        A3_HT_ENTRY(K, V)* prev_entries = table->entries;                                          \
        size_t prev_cap                 = table->cap;                                              \
        table->cap                      = new_cap;                                                 \
        table->entries = (A3_HT_ENTRY(K, V)*)(calloc(table->cap, sizeof(A3_HT_ENTRY(K, V))));      \
                                                                                                   \
        for (size_t i = 0; i < prev_cap; i++) {                                                    \
            A3_HT_ENTRY(K, V)* current_entry = &prev_entries[i];                                   \
            if (!current_entry->hash)                                                              \
                continue;                                                                          \
            A3_HT_INSERT_AT(K, V)                                                                  \
            (table, current_entry->hash, current_entry->key, current_entry->value);                \
        }                                                                                          \
                                                                                                   \
        free(prev_entries);                                                                        \
    }                                                                                              \
                                                                                                   \
    static bool A3_HT_GROW(K, V)(A3_HT(K, V) * table) {                                            \
        assert(table);                                                                             \
        if (!table->can_grow)                                                                      \
            return false;                                                                          \
        A3_HT_RESIZE(K, V)(table, table->cap * 2);                                                 \
        return true;                                                                               \
    }                                                                                              \
                                                                                                   \
    A3_SSIZE_T A3_HT_FIND_INDEX(K, V)(A3_HT(K, V) * table, K key) {                                \
        assert(table);                                                                             \
                                                                                                   \
        uint64_t hash = A3_HT_HASH(K, V)(table, key);                                              \
        for (size_t i = hash % table->cap, probe_count = 0;;                                       \
             i = (i + 1) % table->cap, probe_count++) {                                            \
            A3_HT_ENTRY(K, V)* current_entry = &table->entries[i];                                 \
            if (!current_entry->hash ||                                                            \
                A3_HT_PROBE_COUNT(K, V)(table, i, current_entry->hash) < probe_count)              \
                return -1;                                                                         \
            if (hash == current_entry->hash && C(key, current_entry->key) == 0)                    \
                return (A3_SSIZE_T)i;                                                              \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
    A3_HT_ENTRY(K, V) * A3_HT_FIND_ENTRY(K, V)(A3_HT(K, V) * table, K key) {                       \
        assert(table);                                                                             \
        A3_SSIZE_T i = A3_HT_FIND_INDEX(K, V)(table, key);                                         \
        if (i < 0)                                                                                 \
            return NULL;                                                                           \
        return &table->entries[i];                                                                 \
    }                                                                                              \
                                                                                                   \
    void A3_HT_INIT(K, V)(A3_HT(K, V) * table, bool can_grow) {                                    \
        assert(table);                                                                             \
        A3_STRUCT_ZERO(table);                                                                     \
        table->can_grow    = can_grow;                                                             \
        table->size        = 0;                                                                    \
        table->cap         = A3_HT_INITIAL_CAP;                                                    \
        uint8_t* key_bytes = (uint8_t*)&table->hash_key[0];                                        \
        for (size_t i = 0; i < A3_HT_HASH_KEY_SIZE * sizeof(table->hash_key[0]); i++) {            \
            key_bytes[i] = (uint8_t)rand();                                                        \
        }                                                                                          \
        table->entries = (A3_HT_ENTRY(K, V)*)calloc(table->cap, sizeof(A3_HT_ENTRY(K, V)));        \
        A3_UNWRAPND(table->entries);                                                               \
    }                                                                                              \
                                                                                                   \
    A3_HT(K, V) * A3_HT_NEW(K, V)(bool can_grow) {                                                 \
        A3_HT(K, V)* ret = (A3_HT(K, V)*)calloc(1, sizeof(A3_HT(K, V)));                           \
        A3_HT_INIT(K, V)(ret, can_grow);                                                           \
        return ret;                                                                                \
    }                                                                                              \
                                                                                                   \
    void A3_HT_SET_DUPLICATE_CB(K, V)(A3_HT(K, V) * table, A3_HT_DUP_CB(K, V) cb) {                \
        assert(table);                                                                             \
        table->duplicate_cb = cb;                                                                  \
    }                                                                                              \
                                                                                                   \
    void A3_HT_DESTROY(K, V)(A3_HT(K, V) * table) {                                                \
        assert(table);                                                                             \
        if (table->entries)                                                                        \
            free(table->entries);                                                                  \
    }                                                                                              \
                                                                                                   \
    void A3_HT_FREE(K, V)(A3_HT(K, V) * table) {                                                   \
        assert(table);                                                                             \
        A3_HT_DESTROY(K, V)(table);                                                                \
        free(table);                                                                               \
    }                                                                                              \
                                                                                                   \
    bool A3_HT_INSERT(K, V)(A3_HT(K, V) * table, K key, V value) {                                 \
        assert(table);                                                                             \
                                                                                                   \
        if (table->size * 100 >= table->cap * A3_HT_LOAD_FACTOR)                                   \
            if (!A3_HT_GROW(K, V)(table) && table->size >= table->cap)                             \
                return false;                                                                      \
                                                                                                   \
        table->size++;                                                                             \
        return A3_HT_INSERT_AT(K, V)(table, A3_HT_HASH(K, V)(table, key), key, value);             \
    }                                                                                              \
                                                                                                   \
    V* A3_HT_FIND(K, V)(A3_HT(K, V) * table, K key) {                                              \
        assert(table);                                                                             \
                                                                                                   \
        A3_HT_ENTRY(K, V)* entry = A3_HT_FIND_ENTRY(K, V)(table, key);                             \
        A3_TRYB_MAP(entry, NULL);                                                                  \
        return &entry->value;                                                                      \
    }                                                                                              \
                                                                                                   \
    bool A3_HT_DELETE_INDEX(K, V)(A3_HT(K, V) * table, size_t index) {                             \
        assert(table);                                                                             \
                                                                                                   \
        A3_HT_ENTRY(K, V)* entry = &table->entries[index];                                         \
        A3_TRYB(entry);                                                                            \
        entry->hash = 0;                                                                           \
        table->size--;                                                                             \
                                                                                                   \
        /* Shift the following sequence of entries back. */                                        \
        size_t i                      = ((size_t)index + 1) % table->cap;                          \
        A3_HT_ENTRY(K, V)* next_entry = &table->entries[i];                                        \
        while (next_entry->hash && A3_HT_PROBE_COUNT(K, V)(table, i, next_entry->hash)) {          \
            entry->hash = 0;                                                                       \
            *entry      = *next_entry;                                                             \
            i           = (i + 1) % table->cap;                                                    \
            entry       = next_entry;                                                              \
            next_entry  = &table->entries[i];                                                      \
        }                                                                                          \
        entry->hash = 0;                                                                           \
                                                                                                   \
        return true;                                                                               \
    }                                                                                              \
                                                                                                   \
    bool A3_HT_DELETE(K, V)(A3_HT(K, V) * table, K key) {                                          \
        assert(table);                                                                             \
                                                                                                   \
        A3_SSIZE_T index = A3_HT_FIND_INDEX(K, V)(table, key);                                     \
        if (index < 0)                                                                             \
            return false;                                                                          \
        return A3_HT_DELETE_INDEX(K, V)(table, (size_t)index);                                     \
    }

// Define methods with HighwayHash as the hash function. Helpers have the
// signatures:
//     const uint8_t* KEY_BYTES(K key);
//           size_t   KEY_SIZE(K key);
// See A3_HT_DEFINE_METHODS_HASHER for information on the comparator C.
#define A3_HT_DEFINE_METHODS(K, V, KEY_BYTES, KEY_SIZE, C)                                         \
    static uint64_t A3_HT_DEFAULT_HASH(K, V)(A3_HT(K, V) * table, K key) {                         \
        assert(table);                                                                             \
        return HighwayHash64(KEY_BYTES(key), KEY_SIZE(key), table->hash_key);                      \
    }                                                                                              \
                                                                                                   \
    A3_HT_DEFINE_METHODS_HASHER(K, V, A3_HT_DEFAULT_HASH(K, V), C)

#define A3_HT_FOR_EACH(K, V, T, K_OUT, V_OUT)                                                      \
    A3_SSIZE_T _i    = A3_HT_NEXT_ENTRY(K, V)((T), 0);                                             \
    K*         K_OUT = (_i >= 0) ? &(T)->entries[_i].key : NULL;                                   \
    V*         V_OUT = (_i >= 0) ? &(T)->entries[_i].value : NULL;                                 \
    for (; _i >= 0 && (size_t)_i < (T)->cap; _i    = A3_HT_NEXT_ENTRY(K, V)((T), (size_t)_i + 1),  \
                                             K_OUT = &(T)->entries[MAX(_i, 0)].key,                \
                                             V_OUT = &(T)->entries[MAX(_i, 0)].value)
