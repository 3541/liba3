/*
 * CACHE -- A simple pseudo-LRU cache backed by a hash table.
 *
 * Copyright (c) 2020-2021, Alex O'Brien <3541@3541.website>
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

#define A3_CACHE_ENTRY(K, V)    A3_HT_ENTRY(K, V)
#define A3_CACHE(K, V)          struct K##V##A3Cache
#define A3_CACHE_EVICT_CB(K, V) K##V##A3CacheEvictCallback

#define A3_CACHE_BLOCK_FULL        SIZE_MAX
#define A3_CACHE_ENTRIES_PER_BLOCK (sizeof(size_t) * 8)

#define A3_CACHE_DEFINE_STRUCTS(K, V)                                                              \
    A3_H_BEGIN                                                                                     \
                                                                                                   \
    A3_HT_DEFINE_STRUCTS(K, V)                                                                     \
                                                                                                   \
    typedef void (*A3_CACHE_EVICT_CB(K, V))(void*, K*, V*);                                        \
                                                                                                   \
    A3_CACHE(K, V) {                                                                               \
        size_t eviction_index;                                                                     \
        A3_CACHE_EVICT_CB(K, V) eviction_callback;                                                 \
        size_t* accessed;                                                                          \
        A3_HT(K, V) table;                                                                         \
    };                                                                                             \
                                                                                                   \
    A3_H_END

#define A3_CACHE_INIT(K, V)    K##V##_a3_cache_init
#define A3_CACHE_NEW(K, V)     K##V##_a3_cache_new
#define A3_CACHE_DESTROY(K, V) K##V##_a3_cache_destroy
#define A3_CACHE_FREE(K, V)    K##V##_a3_cache_free

#define A3_CACHE_ACCESS(K, V)   K##V##_a3_cache_access
#define A3_CACHE_ACCESSED(K, V) K##V##_a3_cache_accessed
#define A3_CACHE_EVICT(K, V)    K##V##_a3_cache_evict

#define A3_CACHE_FIND(K, V)   K##V##_a3_cache_find
#define A3_CACHE_INSERT(K, V) K##V##_a3_cache_insert
#define A3_CACHE_CLEAR(K, V)  K##V##_a3_cache_clear

#define A3_CACHE_DECLARE_METHODS(K, V)                                                             \
    A3_H_BEGIN                                                                                     \
                                                                                                   \
    void A3_CACHE_INIT(K, V)(A3_CACHE(K, V)*, size_t capacity, A3_CACHE_EVICT_CB(K, V));           \
    A3_CACHE(K, V) * A3_CACHE_NEW(K, V)(size_t capacity, A3_CACHE_EVICT_CB(K, V));                 \
    void A3_CACHE_DESTROY(K, V)(A3_CACHE(K, V)*);                                                  \
    void A3_CACHE_FREE(K, V)(A3_CACHE(K, V)*);                                                     \
                                                                                                   \
    V*   A3_CACHE_FIND(K, V)(A3_CACHE(K, V)*, K);                                                  \
    void A3_CACHE_INSERT(K, V)(A3_CACHE(K, V)*, K, V, void* callback_ctx);                         \
    void A3_CACHE_CLEAR(K, V)(A3_CACHE(K, V)*, void* callback_ctx);                                \
                                                                                                   \
    A3_HT_DECLARE_METHODS(K, V)                                                                    \
                                                                                                   \
    A3_H_END

#define A3_CACHE_DEFINE_METHODS_NOHT(K, V)                                                         \
    void A3_CACHE_INIT(K, V)(A3_CACHE(K, V) * cache, size_t capacity,                              \
                             A3_CACHE_EVICT_CB(K, V) eviction_callback) {                          \
        assert(cache);                                                                             \
        assert(capacity > 0);                                                                      \
                                                                                                   \
        cache->eviction_index    = 0;                                                              \
        cache->eviction_callback = eviction_callback;                                              \
        A3_UNWRAPN(cache->accessed,                                                                \
                   (size_t*)calloc(capacity / A3_CACHE_ENTRIES_PER_BLOCK, sizeof(size_t)));        \
        A3_HT_INIT(K, V)(&cache->table, A3_HT_NO_HASH_KEY, A3_HT_FORBID_GROWTH);                   \
        A3_HT_RESIZE(K, V)(&cache->table, capacity);                                               \
    }                                                                                              \
                                                                                                   \
    A3_CACHE(K, V) *                                                                               \
        A3_CACHE_NEW(K, V)(size_t capacity, A3_CACHE_EVICT_CB(K, V) eviction_callback) {           \
        assert(capacity > 0);                                                                      \
                                                                                                   \
        A3_CACHE(K, V)* ret = (A3_CACHE(K, V)*)calloc(1, sizeof(A3_CACHE(K, V)));                  \
        if (!ret)                                                                                  \
            return NULL;                                                                           \
        A3_CACHE_INIT(K, V)(ret, capacity, eviction_callback);                                     \
        return ret;                                                                                \
    }                                                                                              \
                                                                                                   \
    void A3_CACHE_DESTROY(K, V)(A3_CACHE(K, V) * cache) {                                          \
        assert(cache);                                                                             \
                                                                                                   \
        free(cache->accessed);                                                                     \
        A3_HT_DESTROY(K, V)(&cache->table);                                                        \
    }                                                                                              \
                                                                                                   \
    void A3_CACHE_FREE(K, V)(A3_CACHE(K, V) * cache) {                                             \
        assert(cache);                                                                             \
                                                                                                   \
        A3_CACHE_DESTROY(K, V)(cache);                                                             \
        free(cache);                                                                               \
    }                                                                                              \
                                                                                                   \
    static void A3_CACHE_ACCESS(K, V)(A3_CACHE(K, V) * cache, size_t index) {                      \
        assert(cache);                                                                             \
        size_t* block = &cache->accessed[index / A3_CACHE_ENTRIES_PER_BLOCK];                      \
        size_t  bit   = 1ULL << (index % A3_CACHE_ENTRIES_PER_BLOCK);                              \
        if ((*block | bit) == A3_CACHE_BLOCK_FULL)                                                 \
            *block = 0ULL;                                                                         \
        *block |= bit;                                                                             \
    }                                                                                              \
                                                                                                   \
    static bool A3_CACHE_ACCESSED(K, V)(A3_CACHE(K, V) * cache, size_t index) {                    \
        assert(cache);                                                                             \
        return cache->accessed[index / A3_CACHE_ENTRIES_PER_BLOCK] &                               \
               (1ULL << (index % A3_CACHE_ENTRIES_PER_BLOCK));                                     \
    }                                                                                              \
                                                                                                   \
    V* A3_CACHE_FIND(K, V)(A3_CACHE(K, V) * cache, K key) {                                        \
        A3_SSIZE_T index = A3_HT_FIND_INDEX(K, V)(&cache->table, key);                             \
        if (index < 0)                                                                             \
            return NULL;                                                                           \
        size_t i = (size_t)index;                                                                  \
        assert(i < cache->table.cap);                                                              \
        A3_CACHE_ACCESS(K, V)(cache, i);                                                           \
                                                                                                   \
        return &cache->table.entries[i].value;                                                     \
    }                                                                                              \
                                                                                                   \
    static void A3_CACHE_EVICT(K, V)(A3_CACHE(K, V) * cache, void* callback_ctx) {                 \
        assert(cache);                                                                             \
                                                                                                   \
        size_t start = cache->eviction_index;                                                      \
        bool   found = false;                                                                      \
        do {                                                                                       \
            if (!A3_CACHE_ACCESSED(K, V)(cache, cache->eviction_index) &&                          \
                cache->table.entries[cache->eviction_index].hash) {                                \
                found = true;                                                                      \
                break; /* Non-recent entry. Evict it. */                                           \
            }                                                                                      \
            cache->eviction_index = (cache->eviction_index + 1) % cache->table.cap;                \
        } while (cache->eviction_index != start);                                                  \
                                                                                                   \
        if (cache->eviction_index == start && !found)                                              \
            A3_PANIC("Unable to evict an entry. This shouldn't be possible.");                     \
        if (cache->eviction_callback) {                                                            \
            A3_HT_ENTRY(K, V)* entry = &cache->table.entries[cache->eviction_index];               \
            cache->eviction_callback(callback_ctx, &entry->key, &entry->value);                    \
        }                                                                                          \
        A3_HT_DELETE_INDEX(K, V)(&cache->table, cache->eviction_index);                            \
        /* The access map needs to be thrown away at this point, since                             \
         * deletion likely caused elements to shift. */                                            \
        memset(cache->accessed, 0,                                                                 \
               cache->table.cap / A3_CACHE_ENTRIES_PER_BLOCK * sizeof(size_t));                    \
    }                                                                                              \
                                                                                                   \
    void A3_CACHE_INSERT(K, V)(A3_CACHE(K, V) * cache, K key, V value, void* callback_ctx) {       \
        assert(cache);                                                                             \
                                                                                                   \
        if (!A3_HT_INSERT(K, V)(&cache->table, key, value)) {                                      \
            A3_CACHE_EVICT(K, V)(cache, callback_ctx);                                             \
            if (!A3_HT_INSERT(K, V)(&cache->table, key, value))                                    \
                A3_PANIC("Unable to insert after eviction.");                                      \
        }                                                                                          \
        A3_CACHE_FIND(K, V)(cache, key);                                                           \
    }                                                                                              \
                                                                                                   \
    void A3_CACHE_CLEAR(K, V)(A3_CACHE(K, V) * cache, void* callback_ctx) {                        \
        assert(cache);                                                                             \
                                                                                                   \
        for (size_t i = 0; i < cache->table.cap; i++) {                                            \
            A3_HT_ENTRY(K, V)* entry = &cache->table.entries[i];                                   \
            if (!entry->hash)                                                                      \
                continue;                                                                          \
            if (cache->eviction_callback)                                                          \
                cache->eviction_callback(callback_ctx, &entry->key, &entry->value);                \
            A3_HT_DELETE_INDEX(K, V)(&cache->table, i);                                            \
        }                                                                                          \
        memset(cache->accessed, 0,                                                                 \
               cache->table.cap / A3_CACHE_ENTRIES_PER_BLOCK * sizeof(size_t));                    \
    }

// See HT.h for information on the latter arguments.
#define A3_CACHE_DEFINE_METHODS(K, V, KEY_BYTES, KEY_SIZE, C)                                      \
    A3_HT_DEFINE_METHODS(K, V, KEY_BYTES, KEY_SIZE, C)                                             \
    A3_CACHE_DEFINE_METHODS_NOHT(K, V)
