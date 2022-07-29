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
#include <stddef.h>

#include <a3/util.h>

/// A list. Keeps track of both the head and tail in order to enable the list to be used as a queue.
#define A3_SLL(NAME, TY)                                                                           \
    struct NAME {                                                                                  \
        TY*  head;                                                                                 \
        TY** end;                                                                                  \
    }

/// A list link. To make a type usable in a list, simply add a member of this type.
#define A3_SLL_LINK(TY)                                                                            \
    struct {                                                                                       \
        TY* next;                                                                                  \
    }

/// Initialize a list.
#define A3_SLL_INIT(LIST)                                                                          \
    do {                                                                                           \
        assert(LIST);                                                                              \
                                                                                                   \
        (LIST)->head = NULL;                                                                       \
        (LIST)->end  = &(LIST)->head;                                                              \
    } while (false)

/// Destroy a list. Does nothing to the contents.
#define A3_SLL_DESTROY A3_SLL_INIT

/// Get the first element of the list. Otherwise, returns `NULL`.
#define A3_SLL_HEAD(LIST) ((LIST)->head)

/// Check whether the list is empty.
#define A3_SLL_IS_EMPTY(LIST) (!A3_SLL_HEAD(LIST))

/// Get the last element of the list. Otherwise, returns `NULL`. THIS MAY BE OUT OF DATE DUE TO
/// CALLS TO ::A3_SLL_INSERT_AFTER.
#define A3_SLL_END(LIST, TY, FIELD)                                                                \
    (!A3_SLL_IS_EMPTY(LIST) ? A3_CONTAINER_OF((LIST)->end, TY, FIELD) : NULL)

/// Get the following element.
#define A3_SLL_NEXT(ELEM, FIELD) ((ELEM)->FIELD.next)

/// Insert the given element after the link.
#define A3_SLL_INSERT_AFTER(ELEM, NEXT_ELEM, FIELD)                                                \
    do {                                                                                           \
        assert(ELEM);                                                                              \
        assert(NEXT_ELEM);                                                                         \
                                                                                                   \
        A3_SLL_NEXT(NEXT_ELEM, FIELD) = A3_SLL_NEXT(ELEM, FIELD);                                  \
        A3_SLL_NEXT(ELEM, FIELD)      = (NEXT_ELEM);                                               \
    } while (false)

/// Remove an item from the head of the list. See also ::A3_SLL_PUSH.
#define A3_SLL_POP(LIST, FIELD)                                                                    \
    do {                                                                                           \
        assert(LIST);                                                                              \
        assert(A3_SLL_HEAD(LIST));                                                                 \
                                                                                                   \
        A3_SLL_HEAD(LIST) = A3_SLL_NEXT(A3_SLL_HEAD(LIST), FIELD);                                 \
        if (!A3_SLL_HEAD(LIST))                                                                    \
            (LIST)->end = &A3_SLL_HEAD(LIST);                                                      \
    } while (false)

/// Remove the given element from the list. Linear time with respect to the length of the list.
#define A3_SLL_REMOVE(LIST, ELEM, TY, FIELD)                                                       \
    do {                                                                                           \
        assert(LIST);                                                                              \
        assert(ELEM);                                                                              \
                                                                                                   \
        if ((ELEM) == A3_SLL_HEAD(LIST)) {                                                         \
            A3_SLL_POP(LIST, FIELD);                                                               \
            break;                                                                                 \
        }                                                                                          \
                                                                                                   \
        TY* _it = A3_SLL_HEAD(LIST);                                                               \
        while (_it && A3_SLL_NEXT(_it, FIELD) != (ELEM))                                           \
            _it = A3_SLL_NEXT(_it, FIELD);                                                         \
        assert(_it);                                                                               \
                                                                                                   \
        A3_SLL_NEXT(_it, FIELD) = A3_SLL_NEXT(A3_SLL_NEXT(_it, FIELD), FIELD);                     \
        if (!A3_SLL_NEXT(_it, FIELD))                                                              \
            (LIST)->end = &A3_SLL_NEXT(_it, FIELD);                                                \
    } while (false)

/// Add an item to the head of the list. See also ::A3_SLL_POP.
#define A3_SLL_PUSH(LIST, ELEM, FIELD)                                                             \
    do {                                                                                           \
        assert(LIST);                                                                              \
        assert(ELEM);                                                                              \
                                                                                                   \
        A3_SLL_NEXT(ELEM, FIELD) = A3_SLL_HEAD(LIST);                                              \
        if (!A3_SLL_NEXT(ELEM, FIELD))                                                             \
            (LIST)->end = &A3_SLL_NEXT(ELEM, FIELD);                                               \
        (LIST)->head = (ELEM);                                                                     \
    } while (false)

/// Fix up the list's end pointer.
#define A3_SLL_FIXUP(LIST, FIELD)                                                                  \
    do {                                                                                           \
        assert(LIST);                                                                              \
                                                                                                   \
        while (*(LIST)->end)                                                                       \
            (LIST)->end = &A3_SLL_NEXT(*(LIST)->end, FIELD);                                       \
    } while (false)

/// \brief Add an item to the end of the list. See also ::A3_SLL_DEQUEUE.
///
/// This fixes up the list's end pointer, if necessary, and so may have worst-case linear time. This
/// is, however, unlikely.
#define A3_SLL_ENQUEUE(LIST, ELEM, FIELD)                                                          \
    do {                                                                                           \
        assert(LIST);                                                                              \
        assert(ELEM);                                                                              \
                                                                                                   \
        A3_SLL_FIXUP(LIST, FIELD);                                                                 \
        A3_SLL_NEXT(ELEM, FIELD) = NULL;                                                           \
        *(LIST)->end             = (ELEM);                                                         \
        (LIST)->end              = &A3_SLL_NEXT(ELEM, FIELD);                                      \
    } while (false)

/// Remove an item from the head of the list.
#define A3_SLL_DEQUEUE A3_SLL_POP

/// Iterate over a list.
#define A3_SLL_FOR_EACH(TY, ITEM, LIST, FIELD)                                                     \
    for (TY* ITEM = A3_SLL_HEAD(LIST), *ITEM##_next = (ITEM) ? A3_SLL_NEXT(ITEM, FIELD) : NULL;    \
         ITEM;                                                                                     \
         ITEM = ITEM##_next, ITEM##_next = ITEM##_next ? A3_SLL_NEXT(ITEM##_next, FIELD) : NULL)
