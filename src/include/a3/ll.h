/*
 * LINKED LIST -- An intrusive doubly-linked list.
 *
 * Copyright (c) 2020-2021, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

/// \file ll.h
/// # Linked List
/// An intrusive doubly-linked list.

#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#include <a3/cpp.h>
#include <a3/platform/types.h>

A3_H_BEGIN

/// A list or a list link. To make a type usable in a list, simply add a member of this type.
typedef struct A3LL {
    struct A3LL* next;
    struct A3LL* prev;
} A3LL;

/// Initialize a list.
A3_ALWAYS_INLINE void a3_ll_init(A3LL* list) {
    assert(list);
    list->next = list->prev = list;
}
/// Destroy a list. Does nothing to the contents.
#define a3_ll_destroy a3_ll_init

/// Allocate and initalize a new list.
A3_EXPORT A3LL* a3_ll_new(void);
/// Free an allocated list. Does nothing to the contents.
A3_EXPORT void a3_ll_free(A3LL*);

/// Check whether the list is empty.
A3_ALWAYS_INLINE bool a3_ll_is_empty(A3LL* list) {
    assert(list);
    return list->next == list;
}

/// Get the first element of the list, if any. Otherwise, returns `NULL`.
A3_ALWAYS_INLINE A3LL* a3_ll_peek(A3LL* list) {
    assert(list);
    return !a3_ll_is_empty(list) ? list->next : NULL;
}

/// Insert the given element after the link.
A3_ALWAYS_INLINE void a3_ll_insert_after(A3LL* link, A3LL* next) {
    assert(link && next);
    assert(link->next && link->prev);
    next->next = link->next;
    link->next = next;
    next->prev = link;
}

/// Remove the given element from the list.
A3_ALWAYS_INLINE void a3_ll_remove(A3LL* link) {
    assert(link);
    assert(link->next && link->prev);
    link->prev->next = link->next;
    link->next->prev = link->prev;
    link->next = link->prev = NULL;
}

/// Enqueue an element on the list.
A3_ALWAYS_INLINE void a3_ll_enqueue(A3LL* list, A3LL* item) {
    assert(list && item);
    a3_ll_insert_after(list->prev, item);
    list->prev = item;
}

/// Dequeue the first element from the list.
A3_ALWAYS_INLINE A3LL* a3_ll_dequeue(A3LL* list) {
    assert(list);
    A3LL* ret = a3_ll_peek(list);
    if (ret)
        a3_ll_remove(ret);
    return ret;
}

/// Iterate over a list.
#define A3_LL_FOREACH(ITEM, LIST)                                                                  \
    for (A3LL* ITEM = (LIST)->next, *_next = ITEM->next; ITEM != (LIST);                           \
         ITEM = _next, _next = _next ? _next->next : NULL)

A3_H_END
