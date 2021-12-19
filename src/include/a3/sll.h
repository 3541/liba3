/*
 * SINGLY-LINKED LIST -- An intrusive singly-linked list.
 *
 * Copyright (c) 2020-2021, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <assert.h>
#include <stdbool.h>

#include <a3/cpp.h>
#include <a3/types.h>
#include <a3/util.h>

A3_H_BEGIN

/// A list link. To make a type usable in a list, simply add a member of this type.
typedef struct A3SLink {
    struct A3SLink* next; ///< The next element in the list.
} A3SLink;

/// A list. Keeps track of both the head and tail in order to enable the list to be used as a queue.
typedef struct {
    A3SLink* head; ///< The first element of the list.
    A3SLink* end;  ///< The last element of the list.
} A3SLL;

/// Initialize a list.
A3_ALWAYS_INLINE void a3_sll_init(A3SLL* list) {
    assert(list);
    list->head = list->end = NULL;
}
/// Destroy a list. Does nothing to the contents.
#define a3_sll_destroy a3_sll_init

/// Allocate and initialize a new list.
A3_EXPORT A3SLL* a3_sll_new(void);
/// Free an allocated list. Does nothing to the contents.
A3_EXPORT void a3_sll_free(A3SLL*);

/// Check whether the list is empty.
A3_ALWAYS_INLINE bool a3_sll_is_empty(A3SLL* list) {
    assert(list);
    return !list->head;
}

/// Get the first element of the list. Otherwise, returns `NULL`.
A3_ALWAYS_INLINE A3SLink* a3_sll_peek(A3SLL* list) {
    assert(list);
    return list->head;
}

/// Insert the given element after the link. An element may only be inserted after the last element
/// in a list.
A3_ALWAYS_INLINE void a3_sll_insert_after(A3SLink* link, A3SLink* next) {
    assert(link && next);
    assert(!link->next);
    link->next = next;
}

/// Remove the given element from the list. Linear time with respect to the length of the list.
A3_EXPORT void a3_sll_remove(A3SLL*, A3SLink*);

/// Add an item to the head of the list. See also ::a3_sll_pop.
A3_ALWAYS_INLINE void a3_sll_push(A3SLL* list, A3SLink* item) {
    assert(list && item);
    assert(!item->next);
    item->next = list->head;
    if (a3_sll_is_empty(list))
        list->end = item;
    list->head = item;
}

/// Remove an item from the head of the list. See also ::a3_sll_push.
A3_ALWAYS_INLINE A3SLink* a3_sll_pop(A3SLL* list) {
    assert(list);
    A3SLink* ret = list->head;
    if (ret) {
        list->head = ret->next;
        if (list->end == ret)
            list->end = NULL;
    }
    return ret;
}

/// Add an item to the end of the list. See also ::a3_sll_dequeue.
A3_EXPORT void a3_sll_enqueue(A3SLL*, A3SLink*);

/// Remove an item from the head of the list. See also ::a3_sll_enqueue.
#define a3_sll_dequeue a3_sll_pop

/// Iterate over a list.
#define A3_SLL_FOREACH(ITEM, LIST)                                                                 \
    for (A3SLink* ITEM = (LIST)->head, *_next = ITEM->next; ITEM;                                  \
         ITEM = _next, _next = _next ? _next->next : NULL)

A3_H_END
