/*
 * CACHE -- A simple pseudo-LRU cache backed by a hash table.
 *
 * Copyright (c) 2020, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <assert.h>
#include <stdlib.h>

#include <a3/cpp.h>
#include <a3/ht.h>
#include <a3/util.h>

#define CACHE_ENTRY(K, V) HT_ENTRY(K, V)
#define CACHE(K, V)       struct K##V##Cache

#define CACHE_BLOCK_FULL        SIZE_MAX
#define CACHE_ENTRIES_PER_BLOCK (sizeof(size_t) * 8)

#define CACHE_DEFINE_STRUCTS(K, V)                                             \
    H_BEGIN                                                                    \
                                                                               \
    HT_DEFINE_STRUCTS(K, V)                                                    \
                                                                               \
    CACHE(K, V) {                                                              \
        size_t  eviction_index;                                                \
        size_t* accessed;                                                      \
        HT(K, V) table;                                                        \
    };                                                                         \
                                                                               \
    H_END

#define CACHE_INIT(K, V)    K##V##_cache_init
#define CACHE_NEW(K, V)     K##V##_cache_new
#define CACHE_DESTROY(K, V) K##V##_cache_destroy
#define CACHE_FREE(K, V)    K##V##_cache_free

#define CACHE_ACCESS(K, V)   K##V##_cache_access
#define CACHE_ACCESSED(K, V) K##V##_cache_accessed
#define CACHE_EVICT(K, V)    K##V##_cache_evict

#define CACHE_FIND(K, V)      K##V##_cache_find
#define CACHE_INSERT(K, V)    K##V##_cache_insert
#define CACHE_EVICT_KEY(K, V) K##V##_cache_evict_key

#define CACHE_DECLARE_METHODS(K, V)                                            \
    H_BEGIN                                                                    \
                                                                               \
    void CACHE_INIT(K, V)(CACHE(K, V)*, size_t capacity);                      \
    CACHE(K, V) * CACHE_NEW(K, V)(size_t capacity);                            \
    void CACHE_DESTROY(K, V)(CACHE(K, V)*);                                    \
    void CACHE_FREE(K, V)(CACHE(K, V)*);                                       \
                                                                               \
    V*   CACHE_FIND(K, V)(CACHE(K, V)*, K);                                    \
    void CACHE_INSERT(K, V)(CACHE(K, V)*, K, V);                               \
    bool CACHE_EVICT_KEY(K, V)(CACHE(K, V)*, K);                               \
                                                                               \
    HT_DECLARE_METHODS(K, V)                                                   \
                                                                               \
    H_END

#define CACHE_DEFINE_METHODS_NOHT(K, V)                                        \
    void CACHE_INIT(K, V)(CACHE(K, V) * cache, size_t capacity) {              \
        assert(cache);                                                         \
        assert(capacity > 0);                                                  \
                                                                               \
        cache->eviction_index = 0;                                             \
        UNWRAPN(cache->accessed, (size_t*)calloc(capacity, sizeof(size_t)));   \
        HT_INIT(K, V)(&cache->table, false);                                   \
        HT_RESIZE(K, V)(&cache->table, capacity);                              \
    }                                                                          \
                                                                               \
    CACHE(K, V) * CACHE_NEW(K, V)(size_t capacity) {                           \
        assert(capacity > 0);                                                  \
                                                                               \
        CACHE(K, V)* ret = (CACHE(K, V)*)calloc(1, sizeof(CACHE(K, V)));       \
        if (!ret)                                                              \
            return NULL;                                                       \
        CACHE_INIT(K, V)(ret, capacity);                                       \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    void CACHE_DESTROY(K, V)(CACHE(K, V) * cache) {                            \
        assert(cache);                                                         \
                                                                               \
        free(cache->accessed);                                                 \
        HT_DESTROY(K, V)(&cache->table);                                       \
    }                                                                          \
                                                                               \
    void CACHE_FREE(K, V)(CACHE(K, V) * cache) {                               \
        assert(cache);                                                         \
                                                                               \
        CACHE_DESTROY(K, V)(cache);                                            \
        free(cache);                                                           \
    }                                                                          \
                                                                               \
    static void CACHE_ACCESS(K, V)(CACHE(K, V) * cache, size_t index) {        \
        assert(cache);                                                         \
        size_t* block = &cache->accessed[index / CACHE_ENTRIES_PER_BLOCK];     \
        size_t  bit   = 1ULL << (index % CACHE_ENTRIES_PER_BLOCK);             \
        if ((*block | bit) == CACHE_BLOCK_FULL)                                \
            *block = 0ULL;                                                     \
        *block |= bit;                                                         \
    }                                                                          \
                                                                               \
    static bool CACHE_ACCESSED(K, V)(CACHE(K, V) * cache, size_t index) {      \
        assert(cache);                                                         \
        return cache->accessed[index / CACHE_ENTRIES_PER_BLOCK] &              \
               (1ULL << (index % CACHE_ENTRIES_PER_BLOCK));                    \
    }                                                                          \
                                                                               \
    V* CACHE_FIND(K, V)(CACHE(K, V) * cache, K key) {                          \
        A3_SSIZE_T index = HT_FIND_INDEX(K, V)(&cache->table, key);            \
        if (index < 0)                                                         \
            return NULL;                                                       \
        size_t i = (size_t)index;                                              \
        assert(i < cache->table.cap);                                          \
        CACHE_ACCESS(K, V)(cache, i);                                          \
                                                                               \
        return &cache->table.entries[i].value;                                 \
    }                                                                          \
                                                                               \
    static void CACHE_EVICT(K, V)(CACHE(K, V) * cache) {                       \
        assert(cache);                                                         \
                                                                               \
        size_t start = cache->eviction_index;                                  \
        bool   found = false;                                                  \
        do {                                                                   \
            if (!CACHE_ACCESSED(K, V)(cache, cache->eviction_index) &&         \
                cache->table.entries[cache->eviction_index].hash) {            \
                found = true;                                                  \
                break; /* Non-recent entry. Evict it. */                       \
            }                                                                  \
            cache->eviction_index =                                            \
                (cache->eviction_index + 1) % cache->table.cap;                \
        } while (cache->eviction_index != start);                              \
                                                                               \
        if (cache->eviction_index == start && !found)                          \
            PANIC("Unable to evict an entry. This shouldn't be possible.");    \
        HT_DELETE_INDEX(K, V)(&cache->table, cache->eviction_index);           \
        /* The access map needs to be thrown away at this point, since         \
         * deletion likely caused elements to shift. */                        \
        memset(cache->accessed, 0,                                             \
               cache->table.cap / CACHE_ENTRIES_PER_BLOCK * sizeof(size_t));   \
    }                                                                          \
                                                                               \
    void CACHE_INSERT(K, V)(CACHE(K, V) * cache, K key, V value) {             \
        assert(cache);                                                         \
                                                                               \
        if (!HT_INSERT(K, V)(&cache->table, key, value)) {                     \
            CACHE_EVICT(K, V)(cache);                                          \
            if (!HT_INSERT(K, V)(&cache->table, key, value))                   \
                PANIC("Unable to insert after eviction.");                     \
        }                                                                      \
        CACHE_FIND(K, V)(cache, key);                                          \
    }

// See HT.h for information on the latter arguments.
#define CACHE_DEFINE_METHODS(K, V, KEY_BYTES, KEY_SIZE, C)                     \
    HT_DEFINE_METHODS(K, V, KEY_BYTES, KEY_SIZE, C)                            \
    CACHE_DEFINE_METHODS_NOHT(K, V)
