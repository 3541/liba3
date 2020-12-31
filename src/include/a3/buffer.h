/*
 * BUFFER -- A growable buffer.
 *
 * Copyright (c) 2020, Alex O'Brien <3541ax@gmail.com>
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
#include <string.h>

#include <a3/cpp.h>
#include <a3/platform/types.h>
#include <a3/str.h>
#include <a3/util.h>

H_BEGIN

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
EXPORT Buffer* buf_new(size_t cap, size_t max_cap);
EXPORT void    buf_destroy(Buffer*);
EXPORT void    buf_free(Buffer*);

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
    Buffer* buf;
    CString delim;
    bool    preserve_end;
} _buf_token_next_args;

EXPORT String buf_token_next_impl(_buf_token_next_args);

#define buf_token_next(BUF, DELIM, ...)                                        \
    buf_token_next_impl((_buf_token_next_args) {                               \
        .buf = (BUF), .delim = (DELIM), .preserve_end = false, __VA_ARGS__ })
#define buf_token_next_copy(BUF, DELIM, ...)                                   \
    string_clone(S_CONST(buf_token_next((BUF), (DELIM), __VA_ARGS__)))

EXPORT inline bool buf_initialized(const Buffer* buf) {
    assert(buf);
    assert(buf->head <= buf->tail);

    return buf->data.ptr;
}

EXPORT inline void buf_reset(Buffer* buf) {
    assert(buf_initialized(buf));

    buf->head = 0;
    buf->tail = 0;
}

EXPORT inline bool buf_reset_if_empty(Buffer* buf) {
    assert(buf_initialized(buf));

    if (buf->head != buf->tail)
        return false;

    buf_reset(buf);
    return true;
}

// Length of the contents of the buffer.
EXPORT inline size_t buf_len(const Buffer* buf) {
    assert(buf_initialized(buf));
    return buf->tail - buf->head;
}

// Total available capacity for writing.
EXPORT inline size_t buf_cap(const Buffer* buf) {
    assert(buf_initialized(buf));
    return buf->data.len - buf_len(buf);
}

// Available space for a single write (i.e., continguous space).
EXPORT inline size_t buf_space(Buffer* buf) {
    assert(buf_initialized(buf));

    buf_reset_if_empty(buf);
    return buf->data.len - buf->tail;
}

// Compact the contents to the start of the buffer.
EXPORT inline bool buf_compact(Buffer* buf) {
    assert(buf_initialized(buf));
    assert(buf->head != 0);

    TRYB(memmove(buf->data.ptr, &buf->data.ptr[buf->head], buf_len(buf)));
    buf->tail -= buf->head;
    buf->head = 0;
    return true;
}

// Attempt to grow the buffer to fit at least min_extra_cap more bytes.
EXPORT inline bool buf_ensure_cap(Buffer* buf, size_t min_extra_cap) {
    assert(buf_initialized(buf));

    if (buf_space(buf) >= min_extra_cap)
        return true;
    // Nope.
    if (buf_len(buf) + min_extra_cap > buf->max_cap)
        return false;

    if (buf_cap(buf) >= min_extra_cap)
        return buf_compact(buf);

    size_t new_cap = buf->data.len;
    for (; new_cap < buf->data.len + min_extra_cap; new_cap *= 2)
        ;
    String new_data = string_realloc(&buf->data, MIN(new_cap, buf->max_cap));
    TRYB(new_data.ptr);
    buf->data = new_data;

    return true;
}

// Attempt to grow the buffer to its maximum capacity.
EXPORT inline bool buf_ensure_max_cap(Buffer* buf) {
    assert(buf_initialized(buf));

    if (buf->data.len >= buf->max_cap)
        return true;

    return buf_ensure_cap(buf, buf->max_cap - buf->data.len);
}

// Pointer for writing into the buffer.
EXPORT inline String buf_write_ptr(Buffer* buf) {
    assert(buf);

    buf_reset_if_empty(buf);
#ifdef __cplusplus
    return { buf->data.ptr + buf->tail, buf_space(buf) };
#else
    return (String) { .ptr = buf->data.ptr + buf->tail, .len = buf_space(buf) };
#endif
}

// Pointer for reading from the buffer.
EXPORT inline CString buf_read_ptr(const Buffer* buf) {
    assert(buf_initialized(buf));
#ifdef __cplusplus
    return { buf->data.ptr + buf->head, buf_len(buf) };
#else
    return (CString) { .ptr = buf->data.ptr + buf->head, .len = buf_len(buf) };
#endif
}

// Bytes have been written into the buffer.
EXPORT inline void buf_wrote(Buffer* buf, size_t len) {
    assert(buf_initialized(buf));
    assert(buf->tail + len <= buf->data.len);

    buf->tail += len;
}

EXPORT inline bool buf_write_str(Buffer* buf, CString str) {
    assert(buf_initialized(buf));

    if (str.len + buf_len(buf) > buf->max_cap)
        return false;
    TRYB(buf_ensure_cap(buf, str.len));

    string_copy(buf_write_ptr(buf), str);
    buf_wrote(buf, str.len);
    return true;
}

H_END
