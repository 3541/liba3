// A priority queue backed by a linked list. This is best suited for cases in
// which it is not typical to add elements to anywhere but the end of the queue.

#pragma once

#include <assert.h>

#include <a3/util.h>

#define LPQ(TY)          struct TY##Lpq
#define LPQ_NODE_PTR(TY) struct TY##LpqNodePtr

#define LPQ_IMPL_STRUCTS(TY)                                                   \
    LPQ_NODE_PTR(TY) {                                                         \
        LPQ_NODE_PTR(TY) * next;                                               \
        LPQ_NODE_PTR(TY) * prev;                                               \
    };                                                                         \
                                                                               \
    LPQ(TY) {                                                                  \
        LPQ_NODE_PTR(TY) head;                                                 \
        LPQ_NODE_PTR(TY) end;                                                  \
    };

// Invoke this macro in the struct body to make it a node.
#define LPQ_NODE(TY) LPQ_NODE_PTR(TY) _lpq_ptr

#define LPQ_NODE_CONTAINER_OF(TY) TY##_lpq_node_container_of
#define LPQ_NODE_INSERT_AFTER(TY) TY##_lpq_node_insert_after

#define LPQ_INIT(TY)         TY##_lpq_init
#define LPQ_PEEK(TY)         TY##_lpq_peek
#define LPQ_INSERT_AFTER(TY) TY##_lpq_insert_after
#define LPQ_REMOVE(TY)       TY##_lpq_remove
#define LPQ_ENQUEUE(TY)      TY##_lpq_enqueue
#define LPQ_DEQUEUE(TY)      TY##_lpq_dequeue

#define LPQ_DECLARE_METHODS(TY)                                                 \
    INLINE TY* LPQ_NODE_CONTAINER_OF(TY)(LPQ_NODE_PTR(TY)*);                    \
    void       LPQ_NODE_INSERT_AFTER(TY)(LPQ_NODE_PTR(TY)*, LPQ_NODE_PTR(TY)*); \
                                                                                \
    void LPQ_INIT(TY)(LPQ(TY)*);                                                \
    TY*  LPQ_PEEK(TY)(LPQ(TY)*);                                                \
    void LPQ_INSERT_AFTER(TY)(TY*, TY*);                                        \
    void LPQ_REMOVE(TY)(TY*);                                                   \
    void LPQ_ENQUEUE(TY)(LPQ(TY)*, TY*);                                        \
    TY*  LPQ_DEQUEUE(TY)(LPQ(TY)*)

// Generate method implementations. C must be a comparator function which
// operates on TY* and returns -1, 0, or 1 if lhs is less than, equal to, or
// greater than rhs, respectively.
#define LPQ_IMPL_METHODS(TY, C)                                                \
    TY* LPQ_NODE_CONTAINER_OF(TY)(LPQ_NODE_PTR(TY) * this) {                   \
        assert(this);                                                          \
        return (TY*)((uintptr_t)this - offsetof(TY, _lpq_ptr));                \
    }                                                                          \
                                                                               \
    void LPQ_NODE_INSERT_AFTER(TY)(LPQ_NODE_PTR(TY) * prev,                    \
                                   LPQ_NODE_PTR(TY) * next) {                  \
        assert(prev);                                                          \
        assert(next);                                                          \
        assert(!next->next && !next->prev);                                    \
                                                                               \
        if (prev->next)                                                        \
            next->next = prev->next;                                           \
        prev->next = next;                                                     \
    }                                                                          \
                                                                               \
    void LPQ_INIT(TY)(LPQ(TY) * this) {                                        \
        assert(this);                                                          \
        memset(this, 0, sizeof(LPQ(TY)));                                      \
        this->head.next = &this->end;                                          \
        this->end.prev  = &this->head;                                         \
    }                                                                          \
                                                                               \
    TY* LPQ_PEEK(TY)(LPQ(TY) * this) {                                         \
        assert(this);                                                          \
        if (!this->head.next)                                                  \
            return NULL;                                                       \
        return LPQ_NODE_CONTAINER_OF(TY)(this->head.next);                     \
    }                                                                          \
                                                                               \
    void LPQ_INSERT_AFTER(TY)(TY * prev, TY * next) {                          \
        assert(prev);                                                          \
        assert(next);                                                          \
        assert(C(prev, next) <= 0);                                            \
        LPQ_NODE_INSERT_AFTER(TY)(&prev->_lpq_ptr, &next->_lpq_ptr);           \
    }                                                                          \
                                                                               \
    void LPQ_REMOVE(TY)(TY * this) {                                           \
        assert(this);                                                          \
                                                                               \
        if (this->_lpq_ptr.prev)                                               \
            this->_lpq_ptr.prev->next = this->_lpq_ptr.next;                   \
        if (this->_lpq_ptr.next)                                               \
            this->_lpq_ptr.next->prev = this->_lpq_ptr.prev;                   \
    }                                                                          \
                                                                               \
    void LPQ_ENQUEUE(TY)(LPQ(TY) * this, TY * item) {                          \
        assert(this);                                                          \
        assert(item);                                                          \
                                                                               \
        if (this->head.next == &this->end ||                                   \
            C(item, LPQ_NODE_CONTAINER_OF(TY)(this->head.next)) <= 0) {        \
            LPQ_NODE_INSERT_AFTER(TY)(&this->head, &item->_lpq_ptr);           \
            return;                                                            \
        }                                                                      \
                                                                               \
        assert(this->end.prev);                                                \
        if (C(item, LPQ_NODE_CONTAINER_OF(TY)(this->end.prev)) >= 0) {         \
            LPQ_NODE_INSERT_AFTER(TY)(this->end.prev, &item->_lpq_ptr);        \
            return;                                                            \
        }                                                                      \
        PANIC("TODO: Traverse to find an appropriate insertion point.");       \
    }                                                                          \
                                                                               \
    TY* LPQ_DEQUEUE(TY)(LPQ(TY) * this) {                                      \
        assert(this);                                                          \
        if (!this->head.next)                                                  \
            return NULL;                                                       \
                                                                               \
        TY* ret = LPQ_NODE_CONTAINER_OF(TY)(this->head.next);                  \
        LPQ_REMOVE(TY)(ret);                                                   \
        return ret;                                                            \
    }
