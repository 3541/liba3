/*
 * BUFFER -- A growable buffer.
 *
 * Copyright (c) 2020, Alex O'Brien
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/param.h>

#include <a3/str.h>

// A growable buffer.
// tail: Index at which to write.
// head: Index from which to read.
// When head == tail, the buffer is empty. In such a condition,
// buf_reset_if_empty will reset both indices to 0.
typedef struct Buffer {
    String data;
    size_t tail;
    size_t head;
    size_t max_cap;
} Buffer;

EXPORT bool buf_init(Buffer*, size_t cap, size_t max_cap);

EXPORT bool buf_write_byte(Buffer*, uint8_t);
EXPORT bool buf_write_line(Buffer*, CString);
EXPORT bool buf_write_vfmt(Buffer*, const char* fmt, va_list);
EXPORT bool buf_write_fmt(Buffer*, const char* fmt, ...);
EXPORT bool buf_write_num(Buffer*, size_t);

EXPORT void   buf_read(Buffer*, size_t);
EXPORT String buf_memmem(Buffer*, CString needle);
EXPORT bool   buf_consume(Buffer*, CString needle);

// A hack for pseudo-optional arguments.
typedef struct _buf_token_next_args {
    Buffer* this;
    CString delim;
    bool    preserve_end;
} _buf_token_next_args;

EXPORT String buf_token_next_impl(_buf_token_next_args);

#define buf_token_next(BUF, DELIM, ...)                                        \
    buf_token_next_impl((_buf_token_next_args) {                               \
        .this = (BUF), .delim = (DELIM), .preserve_end = false, __VA_ARGS__ })
#define buf_token_next_copy(BUF, DELIM, ...)                                   \
    string_clone(S_CONST(buf_token_next((BUF), (DELIM), __VA_ARGS__)))

EXPORT void buf_free(Buffer*);

EXPORT inline bool buf_initialized(const Buffer* this) {
    assert(this);
    assert(this->head <= this->tail);

    return this->data.ptr;
}

EXPORT inline void buf_reset(Buffer* this) {
    assert(buf_initialized(this));

    this->head = 0;
    this->tail = 0;
}

EXPORT inline bool buf_reset_if_empty(Buffer* this) {
    assert(buf_initialized(this));

    if (this->head != this->tail)
        return false;

    buf_reset(this);
    return true;
}

// Length of the contents of the buffer.
EXPORT inline size_t buf_len(const Buffer* this) {
    assert(buf_initialized(this));
    return this->tail - this->head;
}

// Total available capacity for writing.
EXPORT inline size_t buf_cap(const Buffer* this) {
    assert(buf_initialized(this));
    return this->data.len - buf_len(this);
}

// Available space for a single write (i.e., continguous space).
EXPORT inline size_t buf_space(Buffer* this) {
    assert(buf_initialized(this));

    buf_reset_if_empty(this);
    return this->data.len - this->tail;
}

// Compact the contents to the start of the buffer.
EXPORT inline bool buf_compact(Buffer* this) {
    assert(buf_initialized(this));
    assert(this->head != 0);

    return memmove(this->data.ptr, &this->data.ptr[this->head], buf_len(this));
}

// Attempt to grow the buffer to fit at least min_extra_cap more bytes.
EXPORT inline bool buf_ensure_cap(Buffer* this, size_t min_extra_cap) {
    assert(buf_initialized(this));

    if (buf_space(this) >= min_extra_cap)
        return true;
    // Nope.
    if (buf_len(this) + min_extra_cap > this->max_cap)
        return false;

    if (buf_cap(this) >= min_extra_cap)
        return buf_compact(this);

    size_t new_cap = this->data.len;
    for (; new_cap < this->data.len + min_extra_cap; new_cap *= 2)
        ;
    String new_data = string_realloc(this->data, MIN(new_cap, this->max_cap));
    TRYB(new_data.ptr);
    this->data = new_data;

    return true;
}

// Attempt to grow the buffer to its maximum capacity.
EXPORT inline bool buf_ensure_max_cap(Buffer* this) {
    assert(buf_initialized(this));

    if (this->data.len >= this->max_cap)
        return true;

    return buf_ensure_cap(this, this->max_cap - this->data.len);
}

// Pointer for writing into the buffer.
EXPORT inline String buf_write_ptr(Buffer* this) {
    assert(this);

    buf_reset_if_empty(this);
    return (String) { .ptr = this->data.ptr + this->tail,
                      .len = buf_space(this) };
}

// Pointer for reading from the buffer.
EXPORT inline CString buf_read_ptr(const Buffer* this) {
    assert(buf_initialized(this));
    return (CString) { .ptr = this->data.ptr + this->head,
                       .len = buf_len(this) };
}

// Bytes have been written into the buffer.
EXPORT inline void buf_wrote(Buffer* this, size_t len) {
    assert(buf_initialized(this));
    assert(this->tail + len <= this->data.len);

    this->tail += len;
}

EXPORT inline bool buf_write_str(Buffer* this, CString str) {
    assert(buf_initialized(this));

    if (str.len + buf_len(this) > this->max_cap)
        return false;
    TRYB(buf_ensure_cap(this, str.len));

    string_copy(buf_write_ptr(this), str);
    buf_wrote(this, str.len);
    return true;
}
