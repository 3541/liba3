/*
 * SINGLY-LINKED LIST -- An intrusive singly-linked list.
 *
 * Copyright (c) 2021, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#include <a3/sll.h>

#include <stdlib.h>

A3SLL* a3_sll_new() {
    A3_UNWRAPNI(A3SLL*, ret, calloc(1, sizeof(A3SLL)));
    return ret;
}

void a3_sll_free(A3SLL* list) {
    assert(list);
    free(list);
}

void a3_sll_remove(A3SLL* list, A3SLink* item) {
 assert(list && item);
    A3SLink** it = &list->head;
    while (*it && (*it)->next != item)
        it = &(*it)->next;
    assert(*it && (*it)->next);
    *it = item->next;
    item->next = NULL;
    // Defer fixup of ->end to when it's actually needed.
    if (list->end == item)
        list->end = NULL;
}


void a3_sll_enqueue(A3SLL* list, A3SLink* item) {
    assert(list && item);
    A3SLink* end = list->end;
    // If the end isn't present, or isn't up to date, find it.
    if (!a3_sll_is_empty(list) && (!end || end->next)) {
        if (!end)
            end = list->head;
        while (end->next)
            end = end->next;
    }

    if (end)
        end->next = item;
    if (!list->head)
        list->head = item;
    list->end = item;
}
