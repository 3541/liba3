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

#define LL_IMPL_STRUCTS(TY)                                                    \
    H_BEGIN                                                                    \
    LL_NODE_PTR(TY) {                                                          \
        LL_NODE_PTR(TY) * next;                                                \
        LL_NODE_PTR(TY) * prev;                                                \
    };                                                                         \
                                                                               \
    LL(TY) {                                                                   \
        LL_NODE_PTR(TY) head;                                                  \
        LL_NODE_PTR(TY) end;                                                   \
    };                                                                         \
    H_END

// Invoke this macro in the struct body to make it a node.
#define LL_NODE(TY) LL_NODE_PTR(TY) _ll_ptr

#define LL_NODE_CONTAINER_OF(TY) TY##_ll_node_container_of
#define LL_NODE_INSERT_AFTER(TY) TY##_ll_node_insert_after
#define LL_IS_INSERTED(TY)       TY##_ll_is_inserted

#define LL_INIT(TY)         TY##_ll_init
#define LL_PEEK(TY)         TY##_ll_peek
#define LL_INSERT_AFTER(TY) TY##_ll_insert_after
#define LL_REMOVE(TY)       TY##_ll_remove
#define LL_ENQUEUE(TY)      TY##_ll_enqueue
#define LL_DEQUEUE(TY)      TY##_ll_dequeue

#define LL_DECLARE_METHODS(TY)                                                 \
    H_BEGIN                                                                    \
    inline TY*  LL_NODE_CONTAINER_OF(TY)(LL_NODE_PTR(TY)*);                    \
    void        LL_NODE_INSERT_AFTER(TY)(LL_NODE_PTR(TY)*, LL_NODE_PTR(TY)*);  \
    inline bool LL_IS_INSERTED(TY)(TY*);                                       \
                                                                               \
    void LL_INIT(TY)(LL(TY)*);                                                 \
    TY*  LL_PEEK(TY)(LL(TY)*);                                                 \
    void LL_INSERT_AFTER(TY)(TY*, TY*);                                        \
    void LL_REMOVE(TY)(TY*);                                                   \
    void LL_ENQUEUE(TY)(LL(TY)*, TY*);                                         \
    TY*  LL_DEQUEUE(TY)(LL(TY)*);                                              \
    H_END

// Generate method implementations. C must be a comparator function which
// operates on TY* and returns -1, 0, or 1 if lhs is less than, equal to, or
// greater than rhs, respectively.
#define LL_IMPL_METHODS(TY)                                                    \
    TY* LL_NODE_CONTAINER_OF(TY)(LL_NODE_PTR(TY) * this) {                     \
        assert(this);                                                          \
        return CONTAINER_OF(this, TY, _ll_ptr);                                \
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
    bool LL_IS_INSERTED(TY)(TY * this) {                                       \
        assert(this);                                                          \
        return this->_ll_ptr.prev && this->_ll_ptr.next;                       \
    }                                                                          \
                                                                               \
    void LL_INIT(TY)(LL(TY) * this) {                                          \
        assert(this);                                                          \
        memset(this, 0, sizeof(LL(TY)));                                       \
        this->head.next = &this->end;                                          \
        this->end.prev  = &this->head;                                         \
    }                                                                          \
                                                                               \
    TY* LL_PEEK(TY)(LL(TY) * this) {                                           \
        assert(this);                                                          \
        if (!this->head.next || this->head.next == &this->end)                 \
            return NULL;                                                       \
        return LL_NODE_CONTAINER_OF(TY)(this->head.next);                      \
    }                                                                          \
                                                                               \
    void LL_INSERT_AFTER(TY)(TY * prev, TY * next) {                           \
        assert(prev);                                                          \
        assert(next);                                                          \
        LL_NODE_INSERT_AFTER(TY)(&prev->_ll_ptr, &next->_ll_ptr);              \
    }                                                                          \
                                                                               \
    void LL_REMOVE(TY)(TY * this) {                                            \
        assert(this);                                                          \
                                                                               \
        if (this->_ll_ptr.prev)                                                \
            this->_ll_ptr.prev->next = this->_ll_ptr.next;                     \
        if (this->_ll_ptr.next)                                                \
            this->_ll_ptr.next->prev = this->_ll_ptr.prev;                     \
        this->_ll_ptr.next = NULL;                                             \
        this->_ll_ptr.prev = NULL;                                             \
    }                                                                          \
                                                                               \
    void LL_ENQUEUE(TY)(LL(TY) * this, TY * item) {                            \
        assert(this);                                                          \
        assert(item);                                                          \
                                                                               \
        if (this->head.next == &this->end) {                                   \
            LL_NODE_INSERT_AFTER(TY)(&this->head, &item->_ll_ptr);             \
            return;                                                            \
        }                                                                      \
                                                                               \
        assert(this->end.prev&& this->end.prev != &this->head);                \
        LL_NODE_INSERT_AFTER(TY)(this->end.prev, &item->_ll_ptr);              \
    }                                                                          \
                                                                               \
    TY* LL_DEQUEUE(TY)(LL(TY) * this) {                                        \
        assert(this);                                                          \
        if (!this->head.next)                                                  \
            return NULL;                                                       \
                                                                               \
        TY* ret = LL_NODE_CONTAINER_OF(TY)(this->head.next);                   \
        LL_REMOVE(TY)(ret);                                                    \
        return ret;                                                            \
    }
