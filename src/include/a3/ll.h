/*
 * LINKED LIST -- A type-generic intrusive doubly-linked list.
 *
 * Copyright (c) 2020, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <assert.h>

#include <a3/cpp.h>
#include <a3/util.h>

#define LL(TY)          struct TY##LL
#define LL_NODE_PTR(TY) struct TY##LLNodePtr

#define LL_DEFINE_STRUCTS(TY)                                                  \
    H_BEGIN                                                                    \
                                                                               \
    LL_NODE_PTR(TY) {                                                          \
        LL_NODE_PTR(TY) * next;                                                \
        LL_NODE_PTR(TY) * prev;                                                \
    };                                                                         \
                                                                               \
    LL(TY) {                                                                   \
        LL_NODE_PTR(TY) head;                                                  \
        LL_NODE_PTR(TY) end;                                                   \
    };                                                                         \
                                                                               \
    H_END;

// Invoke this macro in the struct body to make it a node.
#define LL_NODE(TY) LL_NODE_PTR(TY) _ll_ptr

#define LL_NODE_CONTAINER_OF(TY) TY##_ll_node_container_of
#define LL_NODE_INSERT_AFTER(TY) TY##_ll_node_insert_after
#define LL_IS_INSERTED(TY)       TY##_ll_is_inserted

#define LL_INIT(TY)    TY##_ll_init
#define LL_NEW(TY)     TY##_ll_new
#define LL_DESTROY(TY) TY##_ll_destroy
#define LL_FREE(TY)    TY##_ll_free

#define LL_PEEK(TY)         TY##_ll_peek
#define LL_NEXT(TY)         TY##_ll_next
#define LL_INSERT_AFTER(TY) TY##_ll_insert_after
#define LL_REMOVE(TY)       TY##_ll_remove
#define LL_ENQUEUE(TY)      TY##_ll_enqueue
#define LL_DEQUEUE(TY)      TY##_ll_dequeue

#define LL_DECLARE_METHODS(TY)                                                 \
    H_BEGIN                                                                    \
                                                                               \
    inline TY*  LL_NODE_CONTAINER_OF(TY)(LL_NODE_PTR(TY)*);                    \
    void        LL_NODE_INSERT_AFTER(TY)(LL_NODE_PTR(TY)*, LL_NODE_PTR(TY)*);  \
    inline bool LL_IS_INSERTED(TY)(TY*);                                       \
                                                                               \
    void LL_INIT(TY)(LL(TY)*);                                                 \
    LL(TY) * LL_NEW(TY)(void);                                                 \
    void LL_DESTROY(TY)(LL(TY)*);                                              \
    void LL_FREE(TY)(LL(TY)*);                                                 \
    TY*  LL_PEEK(TY)(LL(TY)*);                                                 \
    TY*  LL_NEXT(TY)(LL(TY)*, TY*);                                            \
    void LL_INSERT_AFTER(TY)(TY*, TY*);                                        \
    void LL_REMOVE(TY)(TY*);                                                   \
    void LL_ENQUEUE(TY)(LL(TY)*, TY*);                                         \
    TY*  LL_DEQUEUE(TY)(LL(TY)*);                                              \
                                                                               \
    H_END;

// Generate method implementations. C must be a comparator function which
// operates on TY* and returns -1, 0, or 1 if lhs is less than, equal to, or
// greater than rhs, respectively.
#define LL_DEFINE_METHODS(TY)                                                  \
    TY* LL_NODE_CONTAINER_OF(TY)(LL_NODE_PTR(TY) * ptr) {                      \
        assert(ptr);                                                           \
        return CONTAINER_OF(ptr, TY, _ll_ptr);                                 \
    }                                                                          \
                                                                               \
    void LL_NODE_INSERT_AFTER(TY)(LL_NODE_PTR(TY) * prev,                      \
                                  LL_NODE_PTR(TY) * next) {                    \
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
    bool LL_IS_INSERTED(TY)(TY * node) {                                       \
        assert(node);                                                          \
        return node->_ll_ptr.prev && node->_ll_ptr.next;                       \
    }                                                                          \
                                                                               \
    void LL_INIT(TY)(LL(TY) * list) {                                          \
        assert(list);                                                          \
        memset(list, 0, sizeof(LL(TY)));                                       \
        list->head.next = &list->end;                                          \
        list->end.prev  = &list->head;                                         \
    }                                                                          \
                                                                               \
    LL(TY) * LL_NEW(TY)() {                                                    \
        LL(TY)* ret = (LL(TY)*)calloc(1, sizeof(LL(TY)));                      \
        UNWRAPND(ret);                                                         \
        LL_INIT(TY)(ret);                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    void LL_DESTROY(TY)(LL(TY) * list) {                                       \
        assert(list);                                                          \
        memset(list, 0, sizeof(LL(TY)));                                       \
    }                                                                          \
                                                                               \
    void LL_FREE(TY)(LL(TY) * list) {                                          \
        assert(list);                                                          \
        LL_DESTROY(TY)(list);                                                  \
        free(list);                                                            \
    }                                                                          \
                                                                               \
    TY* LL_PEEK(TY)(LL(TY) * list) {                                           \
        assert(list);                                                          \
        if (!list->head.next || list->head.next == &list->end)                 \
            return NULL;                                                       \
        return LL_NODE_CONTAINER_OF(TY)(list->head.next);                      \
    }                                                                          \
                                                                               \
    TY* LL_NEXT(TY)(LL(TY) * list, TY * node) {                                \
        assert(list);                                                          \
        assert(node);                                                          \
        if (!node->_ll_ptr.next || node->_ll_ptr.next == &list->end)           \
            return NULL;                                                       \
        return LL_NODE_CONTAINER_OF(TY)(node->_ll_ptr.next);                   \
    }                                                                          \
                                                                               \
    void LL_INSERT_AFTER(TY)(TY * prev, TY * next) {                           \
        assert(prev);                                                          \
        assert(next);                                                          \
        LL_NODE_INSERT_AFTER(TY)(&prev->_ll_ptr, &next->_ll_ptr);              \
    }                                                                          \
                                                                               \
    void LL_REMOVE(TY)(TY * node) {                                            \
        assert(node);                                                          \
                                                                               \
        if (node->_ll_ptr.prev)                                                \
            node->_ll_ptr.prev->next = node->_ll_ptr.next;                     \
        if (node->_ll_ptr.next)                                                \
            node->_ll_ptr.next->prev = node->_ll_ptr.prev;                     \
        node->_ll_ptr.next = NULL;                                             \
        node->_ll_ptr.prev = NULL;                                             \
    }                                                                          \
                                                                               \
    void LL_ENQUEUE(TY)(LL(TY) * list, TY * item) {                            \
        assert(list);                                                          \
        assert(item);                                                          \
                                                                               \
        if (list->head.next == &list->end) {                                   \
            LL_NODE_INSERT_AFTER(TY)(&list->head, &item->_ll_ptr);             \
            return;                                                            \
        }                                                                      \
                                                                               \
        assert(list->end.prev && list->end.prev != &list->head);               \
        LL_NODE_INSERT_AFTER(TY)(list->end.prev, &item->_ll_ptr);              \
    }                                                                          \
                                                                               \
    TY* LL_DEQUEUE(TY)(LL(TY) * list) {                                        \
        assert(list);                                                          \
        if (!list->head.next || list->head.next == &list->end)                 \
            return NULL;                                                       \
                                                                               \
        TY* ret = LL_NODE_CONTAINER_OF(TY)(list->head.next);                   \
        LL_REMOVE(TY)(ret);                                                    \
        return ret;                                                            \
    }
