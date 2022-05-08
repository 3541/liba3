/*
 * SINGLY-LINKED LIST -- An intrusive singly-linked list.
 *
 * Copyright (c) 2020-2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

/// \file sll.h
/// # Singly-linked list.
/// An intrusive singly-linked list. This is very similar to `STAILQ` from the canonical
/// `sys/queue.h`. The main difference here is that the list's `end` pointer is not kept up to date
/// by ::A3_SLL_INSERT_AFTER, which allows that function to take only the requisite elements as
/// parameters. This is corrected in ::A3_SLL_ENQUEUE.

#pragma once

#include <assert.h>
#include <stdbool.h>

/// A list. Keeps track of both the head and tail in order to enable the list to be used as a queue.
#define A3_SLL(NAME, TY)                                                                           \
    struct NAME {                                                                                  \
        TY*  head;                                                                                 \
        TY** end;                                                                                  \
    }

/// A list link. To make a type usable in a list, simply add a member of this type.
#define A3_S_LINK(TY)                                                                              \
    struct {                                                                                       \
        TY* next;                                                                                  \
    }

/// Initialize a list.
#define A3_SLL_INIT(L)                                                                             \
    do {                                                                                           \
        assert(L);                                                                                 \
        (L)->head = NULL;                                                                          \
        (L)->end  = &(L)->head;                                                                    \
    } while (false)

/// Destroy a list. Does nothing to the contents.
#define A3_SLL_DESTROY A3_SLL_INIT

/// Get the first element of the list. Otherwise, returns `NULL`.
#define A3_SLL_HEAD(L) ((L)->head)

/// Get the following element.
#define A3_SLL_NEXT(E, F) ((E)->F.next)

/// Check whether the list is empty.
#define A3_SLL_IS_EMPTY(L) (!A3_SLL_HEAD(L))

/// Insert the given element after the link.
#define A3_SLL_INSERT_AFTER(A, B, F)                                                               \
    do {                                                                                           \
        assert(A);                                                                                 \
        assert(B);                                                                                 \
        A3_SLL_NEXT(B, F) = A3_SLL_NEXT(A, F);                                                     \
        A3_SLL_NEXT(A, F) = (B);                                                                   \
    } while (false)

/// Remove an item from the head of the list. See also ::A3_SLL_PUSH.
#define A3_SLL_POP(L, F)                                                                           \
    do {                                                                                           \
        assert(L);                                                                                 \
        A3_SLL_HEAD(L) = A3_SLL_NEXT(A3_SLL_HEAD(L), F);                                           \
        if (!A3_SLL_HEAD(L))                                                                       \
            (L)->end = &A3_SLL_HEAD(L);                                                            \
    } while (false)

/// Remove the given element from the list. Linear time with respect to the length of the list.
#define A3_SLL_REMOVE(L, T, E, F)                                                                  \
    do {                                                                                           \
        assert(L);                                                                                 \
        assert(E);                                                                                 \
        if ((E) == A3_SLL_HEAD(L)) {                                                               \
            A3_SLL_POP(L, F);                                                                      \
            break;                                                                                 \
        }                                                                                          \
                                                                                                   \
        T* _it = A3_SLL_HEAD(L);                                                                   \
        while (A3_SLL_NEXT(_it, F) != (E))                                                         \
            _it = A3_SLL_NEXT(_it, F);                                                             \
        A3_SLL_NEXT(_it, F) = A3_SLL_NEXT(A3_SLL_NEXT(_it, F), F);                                 \
        if (!A3_SLL_NEXT(_it, F))                                                                  \
            (L)->end = &A3_SLL_NEXT(_it, F);                                                       \
    } while (false)

/// Add an item to the head of the list. See also ::A3_SLL_POP.
#define A3_SLL_PUSH(L, E, F)                                                                       \
    do {                                                                                           \
        assert(L);                                                                                 \
        assert(E);                                                                                 \
        A3_SLL_NEXT(E, F) = A3_SLL_HEAD(L);                                                        \
        if (!A3_SLL_NEXT(E, F))                                                                    \
            (L)->end = &A3_SLL_NEXT(E, F);                                                         \
        (L)->head = (E);                                                                           \
    } while (false)

/// \brief Add an item to the end of the list. See also ::A3_SLL_DEQUEUE.
///
/// This fixes up the list's end pointer, if necessary, and so may have worst-case linear time. This
/// is, however, unlikely.
#define A3_SLL_ENQUEUE(L, E, F)                                                                    \
    do {                                                                                           \
        assert(L);                                                                                 \
        assert(E);                                                                                 \
        while (*(L)->end)                                                                          \
            (L)->end = &A3_SLL_NEXT(*(L)->end, F);                                                 \
        A3_SLL_NEXT(E, F) = NULL;                                                                  \
        *(L)->end         = (E);                                                                   \
        (L)->end          = &A3_SLL_NEXT(E, F);                                                    \
    } while (false)

/// Remove an item from the head of the list.
#define A3_SLL_DEQUEUE A3_SLL_POP

/// Iterate over a list.
#define A3_SLL_FOR_EACH(TY, ITEM, LIST, FIELD)                                                     \
    if (!A3_SLL_IS_EMPTY(LIST))                                                                    \
        for (TY* ITEM = A3_SLL_HEAD(LIST), *_next = A3_SLL_NEXT(ITEM, FIELD); ITEM;                \
             ITEM = _next, _next = _next ? A3_SLL_NEXT(_next, FIELD) : NULL)
