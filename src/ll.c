/*
 * LINKED LIST -- A type-generic intrusive doubly-linked list.
 *
 * Copyright (c) 2021, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#include <a3/ll.h>

#include <assert.h>
#include <stdlib.h>

#include <a3/util.h>

A3LL* a3_ll_new() {
    A3LL* ret = calloc(1, sizeof(A3LL));
    A3_UNWRAPND(ret);
    a3_ll_init(ret);
    return ret;
}

void a3_ll_free(A3LL* list) {
    assert(list);
    a3_ll_destroy(list);
    free(list);
}
