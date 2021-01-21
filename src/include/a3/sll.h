/*
 * SINGLY-LINKED LIST -- A type-generic intrusive singly-linked list.
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

#define A3_SLL(TY) struct TY##A3SLL

#define A3_SLL_DEFINE_STRUCTS(TY)                                              \
    A3_H_BEGIN                                                                 \
                                                                               \
    A3_SLL(TY) {                                                               \
        TY* head;                                                              \
        TY* end;                                                               \
    };                                                                         \
                                                                               \
    A3_H_END

// Invoke this macro in a struct body to make it a node.
#ifdef __cplusplus
#define A3_SLL_NODE(TY)                                                        \
    TY* _a3_sll_next { nullptr }
#else
#define A3_SLL_NODE(TY) TY* _a3_sll_next
#endif

#define A3_SLL_INIT(TY)    TY##_a3_sll_init
#define A3_SLL_NEW(TY)     TY##_a3_sll_new
#define A3_SLL_DESTROY(TY) TY##_a3_sll_destroy
#define A3_SLL_FREE(TY)    TY##_a3_sll_free

#define A3_SLL_PEEK(TY)    TY##_a3_sll_peek
#define A3_SLL_NEXT(TY)    TY##_a3_sll_next
#define A3_SLL_REMOVE(TY)  TY##_a3_sll_remove
#define A3_SLL_PUSH(TY)    TY##_a3_sll_push
#define A3_SLL_POP(TY)     TY##_a3_sll_pop
#define A3_SLL_ENQUEUE(TY) TY##_a3_sll_enqueue
#define A3_SLL_DEQUEUE(TY) TY##_a3_sll_dequeue

#define A3_SLL_DECLARE_METHODS(TY)                                             \
    A3_H_BEGIN                                                                 \
                                                                               \
    void A3_SLL_INIT(TY)(A3_SLL(TY)*);                                         \
    A3_SLL(TY) * A3_SLL_NEW(TY)(void);                                         \
    void A3_SLL_DESTROY(TY)(A3_SLL(TY)*);                                      \
    void A3_SLL_FREE(TY)(A3_SLL(TY)*);                                         \
                                                                               \
    TY*  A3_SLL_PEEK(TY)(A3_SLL(TY)*);                                         \
    TY*  A3_SLL_NEXT(TY)(TY*);                                                 \
    void A3_SLL_REMOVE(TY)(A3_SLL(TY)*, TY*);                                  \
    void A3_SLL_PUSH(TY)(A3_SLL(TY)*, TY*);                                    \
    TY*  A3_SLL_POP(TY)(A3_SLL(TY)*);                                          \
    void A3_SLL_ENQUEUE(TY)(A3_SLL(TY)*, TY*);                                 \
    TY*  A3_SLL_DEQUEUE(TY)(A3_SLL(TY)*);                                      \
                                                                               \
    A3_H_END;

#define A3_SLL_DEFINE_METHODS(TY)                                              \
    void A3_SLL_INIT(TY)(A3_SLL(TY) * list) {                                  \
        assert(list);                                                          \
        list->head = NULL;                                                     \
        list->end  = NULL;                                                     \
    }                                                                          \
                                                                               \
    A3_SLL(TY) * A3_SLL_NEW(TY)() {                                            \
        A3_SLL(TY)* ret = (A3_SLL(TY)*)calloc(1, sizeof(A3_SLL(TY)));          \
        A3_SLL_INIT(TY)(ret);                                                  \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    void A3_SLL_DESTROY(TY)(A3_SLL(TY) * list) {                               \
        assert(list);                                                          \
        list->head = NULL;                                                     \
        list->end  = NULL;                                                     \
    }                                                                          \
                                                                               \
    void A3_SLL_FREE(TY)(A3_SLL(TY) * list) {                                  \
        assert(list);                                                          \
        A3_SLL_DESTROY(TY)(list);                                              \
    }                                                                          \
                                                                               \
    TY* A3_SLL_PEEK(TY)(A3_SLL(TY) * list) {                                   \
        assert(list);                                                          \
        return list->head;                                                     \
    }                                                                          \
                                                                               \
    TY* A3_SLL_NEXT(TY)(TY * item) {                                           \
        assert(item);                                                          \
        return item->_a3_sll_next;                                             \
    }                                                                          \
                                                                               \
    void A3_SLL_REMOVE(TY)(A3_SLL(TY) * list, TY * item) {                     \
        assert(list);                                                          \
        assert(item);                                                          \
        TY** it = &list->head;                                                 \
        while (*it && *it != item)                                             \
            it = &(*it)->_a3_sll_next;                                         \
        A3_UNWRAPND(*it);                                                      \
        if (*it == list->end)                                                  \
            list->end = NULL;                                                  \
        *it                = item->_a3_sll_next;                               \
        item->_a3_sll_next = NULL;                                             \
    }                                                                          \
                                                                               \
    void A3_SLL_PUSH(TY)(A3_SLL(TY) * list, TY * item) {                       \
        assert(list);                                                          \
        assert(item);                                                          \
        assert(!item->_a3_sll_next);                                           \
        item->_a3_sll_next = list->head;                                       \
        list->head         = item;                                             \
        if (!list->end)                                                        \
            list->end = item;                                                  \
    }                                                                          \
                                                                               \
    TY* A3_SLL_POP(TY)(A3_SLL(TY) * list) {                                    \
        assert(list);                                                          \
        TY* ret = list->head;                                                  \
        if (ret) {                                                             \
            list->head        = ret->_a3_sll_next;                             \
            ret->_a3_sll_next = NULL;                                          \
            if (ret == list->end)                                              \
                list->end = NULL;                                              \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    void A3_SLL_ENQUEUE(TY)(A3_SLL(TY) * list, TY * item) {                    \
        assert(list);                                                          \
        assert(item);                                                          \
        assert(!item->_a3_sll_next);                                           \
        if (list->end)                                                         \
            list->end->_a3_sll_next = item;                                    \
        if (!list->head)                                                       \
            list->head = item;                                                 \
        list->end = item;                                                      \
    }                                                                          \
                                                                               \
    TY* A3_SLL_DEQUEUE(TY)(A3_SLL(TY) * list) { return A3_SLL_POP(TY)(list); }
