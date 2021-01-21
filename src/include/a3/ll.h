/*
 * LINKED LIST -- A type-generic intrusive doubly-linked list.
 *
 * Copyright (c) 2020-2021, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <assert.h>

#include <a3/cpp.h>
#include <a3/util.h>

#define A3_LL(TY)          struct TY##A3LL
#define A3_LL_NODE_PTR(TY) struct TY##A3LLNodePtr

#define A3_LL_DEFINE_STRUCTS(TY)                                               \
    A3_H_BEGIN                                                                 \
                                                                               \
    A3_LL_NODE_PTR(TY) {                                                       \
        A3_LL_NODE_PTR(TY) * next;                                             \
        A3_LL_NODE_PTR(TY) * prev;                                             \
    };                                                                         \
                                                                               \
    A3_LL(TY) {                                                                \
        A3_LL_NODE_PTR(TY) head;                                               \
        A3_LL_NODE_PTR(TY) end;                                                \
    };                                                                         \
                                                                               \
    A3_H_END;

// Invoke this macro in the struct body to make it a node.
#define A3_LL_NODE(TY) A3_LL_NODE_PTR(TY) _a3_ll_ptr

#define A3_LL_NODE_CONTAINER_OF(TY) TY##_a3_ll_node_container_of
#define A3_LL_NODE_INSERT_AFTER(TY) TY##_a3_ll_node_insert_after
#define A3_LL_IS_INSERTED(TY)       TY##_a3_ll_is_inserted

#define A3_LL_INIT(TY)    TY##_a3_ll_init
#define A3_LL_NEW(TY)     TY##_a3_ll_new
#define A3_LL_DESTROY(TY) TY##_a3_ll_destroy
#define A3_LL_FREE(TY)    TY##_a3_ll_free

#define A3_LL_PEEK(TY)         TY##_a3_ll_peek
#define A3_LL_NEXT(TY)         TY##_a3_ll_next
#define A3_LL_INSERT_AFTER(TY) TY##_a3_ll_insert_after
#define A3_LL_REMOVE(TY)       TY##_a3_ll_remove
#define A3_LL_ENQUEUE(TY)      TY##_a3_ll_enqueue
#define A3_LL_DEQUEUE(TY)      TY##_a3_ll_dequeue

#define A3_LL_DECLARE_METHODS(TY)                                              \
    A3_H_BEGIN                                                                 \
                                                                               \
    inline TY*  A3_LL_NODE_CONTAINER_OF(TY)(A3_LL_NODE_PTR(TY)*);              \
    void        A3_LL_NODE_INSERT_AFTER(TY)(A3_LL_NODE_PTR(TY)*,               \
                                     A3_LL_NODE_PTR(TY)*);              \
    inline bool A3_LL_IS_INSERTED(TY)(TY*);                                    \
                                                                               \
    void A3_LL_INIT(TY)(A3_LL(TY)*);                                           \
    A3_LL(TY) * A3_LL_NEW(TY)(void);                                           \
    void A3_LL_DESTROY(TY)(A3_LL(TY)*);                                        \
    void A3_LL_FREE(TY)(A3_LL(TY)*);                                           \
    TY*  A3_LL_PEEK(TY)(A3_LL(TY)*);                                           \
    TY*  A3_LL_NEXT(TY)(A3_LL(TY)*, TY*);                                      \
    void A3_LL_INSERT_AFTER(TY)(TY*, TY*);                                     \
    void A3_LL_REMOVE(TY)(TY*);                                                \
    void A3_LL_ENQUEUE(TY)(A3_LL(TY)*, TY*);                                   \
    TY*  A3_LL_DEQUEUE(TY)(A3_LL(TY)*);                                        \
                                                                               \
    A3_H_END;

// Generate method implementations. C must be a comparator function which
// operates on TY* and returns -1, 0, or 1 if lhs is less than, equal to, or
// greater than rhs, respectively.
#define A3_LL_DEFINE_METHODS(TY)                                               \
    TY* A3_LL_NODE_CONTAINER_OF(TY)(A3_LL_NODE_PTR(TY) * ptr) {                \
        assert(ptr);                                                           \
        return A3_CONTAINER_OF(ptr, TY, _a3_ll_ptr);                           \
    }                                                                          \
                                                                               \
    void A3_LL_NODE_INSERT_AFTER(TY)(A3_LL_NODE_PTR(TY) * prev,                \
                                     A3_LL_NODE_PTR(TY) * next) {              \
        assert(prev);                                                          \
        assert(next);                                                          \
        assert(!next->next && !next->prev);                                    \
                                                                               \
        if (prev->next) {                                                      \
            next->next       = prev->next;                                     \
            next->next->prev = next;                                           \
        }                                                                      \
        prev->next = next;                                                     \
        next->prev = prev;                                                     \
    }                                                                          \
                                                                               \
    bool A3_LL_IS_INSERTED(TY)(TY * node) {                                    \
        assert(node);                                                          \
        return node->_a3_ll_ptr.prev && node->_a3_ll_ptr.next;                 \
    }                                                                          \
                                                                               \
    void A3_LL_INIT(TY)(A3_LL(TY) * list) {                                    \
        assert(list);                                                          \
        memset(list, 0, sizeof(A3_LL(TY)));                                    \
        list->head.next = &list->end;                                          \
        list->end.prev  = &list->head;                                         \
    }                                                                          \
                                                                               \
    A3_LL(TY) * A3_LL_NEW(TY)() {                                              \
        A3_LL(TY)* ret = (A3_LL(TY)*)calloc(1, sizeof(A3_LL(TY)));             \
        A3_UNWRAPND(ret);                                                      \
        A3_LL_INIT(TY)(ret);                                                   \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    void A3_LL_DESTROY(TY)(A3_LL(TY) * list) {                                 \
        assert(list);                                                          \
        memset(list, 0, sizeof(A3_LL(TY)));                                    \
    }                                                                          \
                                                                               \
    void A3_LL_FREE(TY)(A3_LL(TY) * list) {                                    \
        assert(list);                                                          \
        A3_LL_DESTROY(TY)(list);                                               \
        free(list);                                                            \
    }                                                                          \
                                                                               \
    TY* A3_LL_PEEK(TY)(A3_LL(TY) * list) {                                     \
        assert(list);                                                          \
        if (!list->head.next || list->head.next == &list->end)                 \
            return NULL;                                                       \
        return A3_LL_NODE_CONTAINER_OF(TY)(list->head.next);                   \
    }                                                                          \
                                                                               \
    TY* A3_LL_NEXT(TY)(A3_LL(TY) * list, TY * node) {                          \
        assert(list);                                                          \
        assert(node);                                                          \
        if (!node->_a3_ll_ptr.next || node->_a3_ll_ptr.next == &list->end)     \
            return NULL;                                                       \
        return A3_LL_NODE_CONTAINER_OF(TY)(node->_a3_ll_ptr.next);             \
    }                                                                          \
                                                                               \
    void A3_LL_INSERT_AFTER(TY)(TY * prev, TY * next) {                        \
        assert(prev);                                                          \
        assert(next);                                                          \
        A3_LL_NODE_INSERT_AFTER(TY)(&prev->_a3_ll_ptr, &next->_a3_ll_ptr);     \
    }                                                                          \
                                                                               \
    void A3_LL_REMOVE(TY)(TY * node) {                                         \
        assert(node);                                                          \
                                                                               \
        if (node->_a3_ll_ptr.prev)                                             \
            node->_a3_ll_ptr.prev->next = node->_a3_ll_ptr.next;               \
        if (node->_a3_ll_ptr.next)                                             \
            node->_a3_ll_ptr.next->prev = node->_a3_ll_ptr.prev;               \
        node->_a3_ll_ptr.next = NULL;                                          \
        node->_a3_ll_ptr.prev = NULL;                                          \
    }                                                                          \
                                                                               \
    void A3_LL_ENQUEUE(TY)(A3_LL(TY) * list, TY * item) {                      \
        assert(list);                                                          \
        assert(item);                                                          \
                                                                               \
        if (list->head.next == &list->end) {                                   \
            A3_LL_NODE_INSERT_AFTER(TY)(&list->head, &item->_a3_ll_ptr);       \
            return;                                                            \
        }                                                                      \
                                                                               \
        assert(list->end.prev && list->end.prev != &list->head);               \
        A3_LL_NODE_INSERT_AFTER(TY)(list->end.prev, &item->_a3_ll_ptr);        \
    }                                                                          \
                                                                               \
    TY* A3_LL_DEQUEUE(TY)(A3_LL(TY) * list) {                                  \
        assert(list);                                                          \
        if (!list->head.next || list->head.next == &list->end)                 \
            return NULL;                                                       \
                                                                               \
        TY* ret = A3_LL_NODE_CONTAINER_OF(TY)(list->head.next);                \
        A3_LL_REMOVE(TY)(ret);                                                 \
        return ret;                                                            \
    }
