/*
 * SINGLY-LINKED LIST -- A type-generic intrusive singly-linked list.
 *
 * Copyright (c) 2020, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <assert.h>

#include <a3/cpp.h>

#define SLL(TY) struct TY##SLL
//#define SLL_NODE_PTR(TY) struct TY##SLLNodePtr

#define SLL_DEFINE_STRUCTS(TY)                                                 \
    H_BEGIN                                                                    \
                                                                               \
    SLL(TY) {                                                                  \
        TY* head;                                                              \
        TY* end;                                                               \
    };                                                                         \
                                                                               \
    H_END

// Invoke this macro in a struct body to make it a node.
#define SLL_NODE(TY) TY* _sll_next

#define SLL_INIT(TY)    TY##_sll_init
#define SLL_NEW(TY)     TY##_sll_new
#define SLL_DESTROY(TY) TY##_sll_destroy
#define SLL_FREE(TY)    TY##_sll_free

#define SLL_PEEK(TY)    TY##_sll_peek
#define SLL_NEXT(TY)    TY##_sll_next
#define SLL_REMOVE(TY)  TY##_sll_remove
#define SLL_PUSH(TY)    TY##_sll_push
#define SLL_POP(TY)     TY##_sll_pop
#define SLL_ENQUEUE(TY) TY##_sll_enqueue
#define SLL_DEQUEUE(TY) TY##_sll_dequeue

#define SLL_DECLARE_METHODS(TY)                                                \
    H_BEGIN                                                                    \
                                                                               \
    void SLL_INIT(TY)(SLL(TY)*);                                               \
    SLL(TY) * SLL_NEW(TY)(void);                                               \
    void SLL_DESTROY(TY)(SLL(TY)*);                                            \
    void SLL_FREE(TY)(SLL(TY)*);                                               \
                                                                               \
    TY*  SLL_PEEK(TY)(SLL(TY)*);                                               \
    TY*  SLL_NEXT(TY)(TY*);                                                    \
    void SLL_PUSH(TY)(SLL(TY)*, TY*);                                          \
    TY*  SLL_POP(TY)(SLL(TY)*);                                                \
    void SLL_ENQUEUE(TY)(SLL(TY)*, TY*);                                       \
    TY*  SLL_DEQUEUE(TY)(SLL(TY)*);                                            \
                                                                               \
    H_END;

#define SLL_DEFINE_METHODS(TY)                                                 \
    void SLL_INIT(TY)(SLL(TY) * list) {                                        \
        assert(list);                                                          \
        list->head = NULL;                                                     \
        list->end  = NULL;                                                     \
    }                                                                          \
                                                                               \
    SLL(TY) * SLL_NEW(TY)() {                                                  \
        SLL(TY)* ret = (SLL(TY)*)calloc(1, sizeof(SLL(TY)));                   \
        SLL_INIT(TY)(ret);                                                     \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    void SLL_DESTROY(TY)(SLL(TY) * list) {                                     \
        assert(list);                                                          \
        list->head = NULL;                                                     \
        list->end  = NULL;                                                     \
    }                                                                          \
                                                                               \
    void SLL_FREE(TY)(SLL(TY) * list) {                                        \
        assert(list);                                                          \
        SLL_DESTROY(TY)(list);                                                 \
    }                                                                          \
                                                                               \
    TY* SLL_PEEK(TY)(SLL(TY) * list) {                                         \
        assert(list);                                                          \
        return list->head;                                                     \
    }                                                                          \
                                                                               \
    TY* SLL_NEXT(TY)(TY * node) {                                              \
        assert(node);                                                          \
        return node->_sll_next;                                                \
    }                                                                          \
                                                                               \
    void SLL_PUSH(TY)(SLL(TY) * list, TY * item) {                             \
        assert(list);                                                          \
        assert(item);                                                          \
        assert(!item->_sll_next);                                              \
        item->_sll_next = list->head;                                          \
        list->head      = item;                                                \
        if (!list->end)                                                        \
            list->end = item;                                                  \
    }                                                                          \
                                                                               \
    TY* SLL_POP(TY)(SLL(TY) * list) {                                          \
        assert(list);                                                          \
        TY* ret = list->head;                                                  \
        if (ret) {                                                             \
            list->head     = ret->_sll_next;                                   \
            ret->_sll_next = NULL;                                             \
            if (ret == list->end)                                              \
                list->end = NULL;                                              \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    void SLL_ENQUEUE(TY)(SLL(TY) * list, TY * item) {                          \
        assert(list);                                                          \
        assert(item);                                                          \
        assert(!item->_sll_next);                                              \
        if (list->end)                                                         \
            list->end->_sll_next = item;                                       \
        if (!list->head)                                                       \
            list->head = item;                                                 \
        list->end = item;                                                      \
    }                                                                          \
                                                                               \
    TY* SLL_DEQUEUE(TY)(SLL(TY) * list) { return SLL_POP(TY)(list); }
