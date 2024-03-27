/*
 * LINKED LIST -- An intrusive doubly-linked list.
 *
 * Copyright (c) 2020-2022, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

/// \file ll.h
/// # Linked List
/// An intrusive doubly-linked list. This is very similar to `TAILQ` from the canonical
/// `sys/queue.h`. The main difference is that it does not require the list itself to be given as a
/// paramter to ::A3_LL_REMOVE, ::A3_LL_INSERT_BEFORE, and ::A3_LL_INSERT_AFTER. This is
/// accomplished by way of a dummy node stored in the list itself.

#pragma once

#include <assert.h>
#include <stddef.h>

#include <a3/cpp.h>
#include <a3/macro.h>
#include <a3/util.h>

A3_H_BEGIN

typedef struct A3LLErasedLink_ {
    void*  next;
    void** prev;
} A3LLErasedLink_;

A3_H_END

/// A list. Keeps track of both the head and tail in order to enable the list to be used as a queue.
#ifndef __cplusplus
#define A3_LL(NAME, TY)                                                                            \
    struct NAME {                                                                                  \
        TY*    head;                                                                               \
        void** dummy_; /* Detectable when traversing backwards. Always NULL. */                    \
        TY     end;                                                                                \
    }
#else
#define A3_LL(NAME, TY)                                                                            \
    struct NAME {                                                                                  \
        TY*    head   = nullptr;                                                                   \
        void** dummy_ = nullptr;                                                                   \
        union {                                                                                    \
            TY             end;                                                                    \
            std::nullptr_t fake_ = nullptr;                                                        \
        };                                                                                         \
    }
#endif

/// A list link. To make a type usable in a list, simply add a member of this type.
#define A3_LL_LINK(TY)                                                                             \
    struct {                                                                                       \
        TY*  next;                                                                                 \
        TY** prev;                                                                                 \
    }

/// Initialize a list.
#define A3_LL_INIT(LIST, FIELD)                                                                    \
    A3_M_BEGIN                                                                                     \
        assert(LIST);                                                                              \
                                                                                                   \
        (LIST)->head           = NULL;                                                             \
        (LIST)->dummy_         = NULL;                                                             \
        (LIST)->end.FIELD.next = NULL;                                                             \
        (LIST)->end.FIELD.prev = &(LIST)->head;                                                    \
    A3_M_END

/// Destroy a list. Does nothing to the contents.
#define A3_LL_DESTROY A3_LL_INIT

/// Check whether the list is empty.
#define A3_LL_IS_EMPTY(LIST) (!(LIST)->head || (LIST)->head == &(LIST)->end)

/// Get the first element of the list. Returns `NULL` if none.
#define A3_LL_HEAD(LIST) (!A3_LL_IS_EMPTY(LIST) ? (LIST)->head : NULL)

/// Get the last element of the list. If none, returns `NULL`.
#define A3_LL_END(LIST, TY, FIELD)                                                                 \
    (!A3_LL_IS_EMPTY(LIST) ? A3_CONTAINER_OF((LIST)->end.FIELD.prev, TY, FIELD) : NULL)

/// Checks whether the given element is the last in the list.
#define A3_LL_IS_LAST(ELEM, FIELD) (assert((ELEM)->FIELD.next), !(ELEM)->FIELD.next->FIELD.next)

/// Get the following element.
#define A3_LL_NEXT(ELEM, FIELD)                                                                    \
    (((ELEM)->FIELD.next && !A3_LL_IS_LAST(ELEM, FIELD)) ? (ELEM)->FIELD.next : NULL)

/// Checks whether the given element is first in the list.
#define A3_LL_IS_FIRST(ELEM, FIELD) (!((A3LLErasedLink_*)(ELEM)->FIELD.prev)->prev)

/// Get the previous element.
#define A3_LL_PREV(ELEM, TY, FIELD)                                                                \
    (!A3_LL_IS_FIRST(ELEM, FIELD) ? A3_CONTAINER_OF((ELEM)->FIELD.prev, TY, FIELD) : NULL)

/// Insert the given element after the current.
#define A3_LL_INSERT_AFTER(ELEM, NEXT_ELEM, FIELD)                                                 \
    A3_M_BEGIN                                                                                     \
        assert(ELEM);                                                                              \
        assert(NEXT_ELEM);                                                                         \
                                                                                                   \
        (NEXT_ELEM)->FIELD.next        = (ELEM)->FIELD.next;                                       \
        (NEXT_ELEM)->FIELD.prev        = &(ELEM)->FIELD.next;                                      \
        (ELEM)->FIELD.next->FIELD.prev = &(NEXT_ELEM)->FIELD.next;                                 \
        (ELEM)->FIELD.next             = (NEXT_ELEM);                                              \
    A3_M_END

/// Insert the given element before the current.
#define A3_LL_INSERT_BEFORE(ELEM, PREV_ELEM, FIELD)                                                \
    A3_M_BEGIN                                                                                     \
        assert(ELEM);                                                                              \
        assert(PREV_ELEM);                                                                         \
                                                                                                   \
        (PREV_ELEM)->FIELD.next = (ELEM);                                                          \
        (PREV_ELEM)->FIELD.prev = (ELEM)->FIELD.prev;                                              \
        *(ELEM)->FIELD.prev     = (PREV_ELEM);                                                     \
        (ELEM)->FIELD.prev      = &(PREV_ELEM)->FIELD.next;                                        \
    A3_M_END

/// Remove the given element from the list.
#define A3_LL_REMOVE(ELEM, FIELD)                                                                  \
    A3_M_BEGIN                                                                                     \
        assert(ELEM);                                                                              \
                                                                                                   \
        (ELEM)->FIELD.next->FIELD.prev = (ELEM)->FIELD.prev;                                       \
        *(ELEM)->FIELD.prev            = (ELEM)->FIELD.next;                                       \
    A3_M_END

/// Add an item to the head of the list. See also ::A3_LL_POP.
#define A3_LL_PUSH(LIST, ELEM, FIELD)                                                              \
    A3_M_BEGIN                                                                                     \
        assert(LIST);                                                                              \
        assert(ELEM);                                                                              \
                                                                                                   \
        if (!A3_LL_IS_EMPTY(LIST)) {                                                               \
            A3_LL_HEAD(LIST)->FIELD.prev = &(ELEM)->FIELD.next;                                    \
            (ELEM)->FIELD.next           = A3_LL_HEAD(LIST);                                       \
        } else {                                                                                   \
            (LIST)->end.FIELD.prev = &(ELEM)->FIELD.next;                                          \
            (ELEM)->FIELD.next     = &(LIST)->end;                                                 \
        }                                                                                          \
                                                                                                   \
        (ELEM)->FIELD.prev = &(LIST)->head;                                                        \
        (LIST)->head       = (ELEM);                                                               \
    A3_M_END

/// Add an item to the end of the list. See also ::A3_LL_DEQUEUE.
#define A3_LL_ENQUEUE(LIST, ELEM, FIELD)                                                           \
    A3_M_BEGIN                                                                                     \
        assert(LIST);                                                                              \
        assert(ELEM);                                                                              \
                                                                                                   \
        (ELEM)->FIELD.next      = &(LIST)->end;                                                    \
        (ELEM)->FIELD.prev      = (LIST)->end.FIELD.prev;                                          \
        *(LIST)->end.FIELD.prev = (ELEM);                                                          \
        (LIST)->end.FIELD.prev  = &(ELEM)->FIELD.next;                                             \
    A3_M_END

/// Remove an item from the head of the list. See also ::A3_LL_PUSH.
#define A3_LL_POP(LIST, FIELD)                                                                     \
    A3_M_BEGIN                                                                                     \
        assert(LIST);                                                                              \
                                                                                                   \
        (LIST)->head = A3_LL_NEXT(A3_LL_HEAD(LIST), FIELD);                                        \
        if (!A3_LL_IS_EMPTY(LIST))                                                                 \
            A3_LL_HEAD(LIST)->FIELD.prev = &(LIST)->head;                                          \
        else                                                                                       \
            (LIST)->end.FIELD.prev = &(LIST)->head;                                                \
    A3_M_END

/// Remove an item from the head of the list. See also ::A3_LL_ENQUEUE.
#define A3_LL_DEQUEUE A3_LL_POP

/// Iterate over a list.
#define A3_LL_FOR_EACH(TY, ITEM, LIST, FIELD)                                                      \
    for (TY* ITEM = A3_LL_HEAD(LIST), *ITEM##_next = (ITEM) ? A3_LL_NEXT(ITEM, FIELD) : NULL;      \
         ITEM;                                                                                     \
         ITEM = ITEM##_next, ITEM##_next = ITEM##_next ? A3_LL_NEXT(ITEM##_next, FIELD) : NULL)

/// Iterate over a list backwards.
#define A3_LL_FOR_EACH_REV(TY, ITEM, LIST, FIELD)                                                  \
    for (TY* ITEM         = A3_LL_END(LIST, TY, FIELD),                                            \
             *ITEM##_prev = (ITEM) ? A3_LL_PREV(ITEM, TY, FIELD) : NULL;                           \
         ITEM; ITEM       = ITEM##_prev,                                                           \
             ITEM##_prev  = ITEM##_prev ? A3_LL_PREV(ITEM##_prev, TY, FIELD) : NULL)
