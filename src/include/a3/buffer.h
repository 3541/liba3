/*
 * BUFFER -- A growable buffer.
 *
 * Copyright (c) 2020-2021, Alex O'Brien <3541ax@gmail.com>
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

A3_H_BEGIN

// A growable buffer.
// tail: Index at which to write.
// head: Index from which to read.
// When head == tail, the buffer is empty. In such a condition,
// a3_buf_reset_if_empty will reset both indices to 0.
typedef struct A3Buffer {
    A3String data;
    size_t   tail;
    size_t   head;
    size_t   max_cap;
} A3Buffer;

A3_EXPORT bool a3_buf_init(A3Buffer*, size_t cap, size_t max_cap);
A3_EXPORT A3Buffer* a3_buf_new(size_t cap, size_t max_cap);
A3_EXPORT void      a3_buf_destroy(A3Buffer*);
A3_EXPORT void      a3_buf_free(A3Buffer*);

A3_EXPORT bool a3_buf_write_byte(A3Buffer*, uint8_t);
A3_EXPORT bool a3_buf_write_line(A3Buffer*, A3CString);
A3_EXPORT bool a3_buf_write_vfmt(A3Buffer*, const char* fmt, va_list);
A3_EXPORT bool a3_buf_write_fmt(A3Buffer*, const char* fmt, ...);
A3_EXPORT bool a3_buf_write_num(A3Buffer*, size_t);

A3_EXPORT void     a3_buf_read(A3Buffer*, size_t);
A3_EXPORT A3String a3_buf_memmem(A3Buffer*, A3CString needle);
A3_EXPORT bool     a3_buf_consume(A3Buffer*, A3CString needle);

// A hack for pseudo-optional arguments.
typedef struct _a3_buf_token_next_args {
    A3Buffer* buf;
    A3CString delim;
    bool      preserve_end;
} _a3_buf_token_next_args;

A3_EXPORT A3String a3_buf_token_next_impl(_a3_buf_token_next_args);

#define a3_buf_token_next(BUF, DELIM, ...)                                                         \
    a3_buf_token_next_impl((_a3_buf_token_next_args) {                                             \
        .buf = (BUF), .delim = (DELIM), .preserve_end = false, __VA_ARGS__ })
#define a3_buf_token_next_copy(BUF, DELIM, ...)                                                    \
    a3_string_clone(A3_S_CONST(a3_buf_token_next((BUF), (DELIM), __VA_ARGS__)))

A3_EXPORT inline bool a3_buf_initialized(const A3Buffer* buf) {
    assert(buf);
    assert(buf->head <= buf->tail);

    return buf->data.ptr;
}

A3_EXPORT inline void a3_buf_reset(A3Buffer* buf) {
    assert(a3_buf_initialized(buf));

    buf->head = 0;
    buf->tail = 0;
}

A3_EXPORT inline bool a3_buf_reset_if_empty(A3Buffer* buf) {
    assert(a3_buf_initialized(buf));

    if (buf->head != buf->tail)
        return false;

    a3_buf_reset(buf);
    return true;
}

// Length of the contents of the buffer.
A3_EXPORT inline size_t a3_buf_len(const A3Buffer* buf) {
    assert(a3_buf_initialized(buf));
    return buf->tail - buf->head;
}

// Total available capacity for writing.
A3_EXPORT inline size_t a3_buf_cap(const A3Buffer* buf) {
    assert(a3_buf_initialized(buf));
    return buf->data.len - a3_buf_len(buf);
}

// Available space for a single write (i.e., continguous space).
A3_EXPORT inline size_t a3_buf_space(A3Buffer* buf) {
    assert(a3_buf_initialized(buf));

    a3_buf_reset_if_empty(buf);
    return buf->data.len - buf->tail;
}

// Compact the contents to the start of the buffer.
A3_EXPORT inline bool a3_buf_compact(A3Buffer* buf) {
    assert(a3_buf_initialized(buf));
    assert(buf->head != 0);

    A3_TRYB(memmove(buf->data.ptr, &buf->data.ptr[buf->head], a3_buf_len(buf)));
    buf->tail -= buf->head;
    buf->head = 0;
    return true;
}

// Attempt to grow the buffer to fit at least min_extra_cap more bytes.
A3_EXPORT inline bool a3_buf_ensure_cap(A3Buffer* buf, size_t min_extra_cap) {
    assert(a3_buf_initialized(buf));

    if (a3_buf_space(buf) >= min_extra_cap)
        return true;
    // Nope.
    if (a3_buf_len(buf) + min_extra_cap > buf->max_cap)
        return false;

    if (a3_buf_cap(buf) >= min_extra_cap)
        return a3_buf_compact(buf);

    size_t new_cap = buf->data.len;
    for (; new_cap < buf->data.len + min_extra_cap; new_cap *= 2)
        ;
    A3String new_data = a3_string_realloc(&buf->data, MIN(new_cap, buf->max_cap));
    A3_TRYB(new_data.ptr);
    buf->data = new_data;

    return true;
}

// Attempt to grow the buffer to its maximum capacity.
A3_EXPORT inline bool a3_buf_ensure_max_cap(A3Buffer* buf) {
    assert(a3_buf_initialized(buf));

    if (buf->data.len >= buf->max_cap)
        return true;

    return a3_buf_ensure_cap(buf, buf->max_cap - buf->data.len);
}

// Pointer for writing into the buffer.
A3_EXPORT inline A3String a3_buf_write_ptr(A3Buffer* buf) {
    assert(buf);

    a3_buf_reset_if_empty(buf);
#ifdef __cplusplus
    return { buf->data.ptr + buf->tail, a3_buf_space(buf) };
#else
    return (A3String) { .ptr = buf->data.ptr + buf->tail, .len = a3_buf_space(buf) };
#endif
}

// Pointer for reading from the buffer.
A3_EXPORT inline A3CString a3_buf_read_ptr(const A3Buffer* buf) {
    assert(a3_buf_initialized(buf));
#ifdef __cplusplus
    return { buf->data.ptr + buf->head, a3_buf_len(buf) };
#else
    return (A3CString) { .ptr = buf->data.ptr + buf->head, .len = a3_buf_len(buf) };
#endif
}

// Bytes have been written into the buffer.
A3_EXPORT inline void a3_buf_wrote(A3Buffer* buf, size_t len) {
    assert(a3_buf_initialized(buf));
    assert(buf->tail + len <= buf->data.len);

    buf->tail += len;
}

A3_EXPORT inline bool a3_buf_write_str(A3Buffer* buf, A3CString str) {
    assert(a3_buf_initialized(buf));

    if (str.len + a3_buf_len(buf) > buf->max_cap)
        return false;
    A3_TRYB(a3_buf_ensure_cap(buf, str.len));

    a3_string_copy(a3_buf_write_ptr(buf), str);
    a3_buf_wrote(buf, str.len);
    return true;
}

#define A3_BUF_WRITE_STRUCT(BUF, S) a3_buf_write_str((BUF), A3_CSS((S)))

A3_H_END
