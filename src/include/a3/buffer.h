/*
 * BUFFER -- A growable buffer.
 *
 * Copyright (c) 2020-2021, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

/// \file buffer.h
/// # Buffer
/// A growable byte buffer.

#pragma once

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <a3/cpp.h>
#include <a3/str.h>
#include <a3/types.h>
#include <a3/util.h>

A3_H_BEGIN

/// \brief A growable buffer.
///
/// When head == tail, the buffer is empty. In such a condition, ::a3_buf_reset_if_empty will reset
/// both indices to 0.
typedef struct A3Buffer {
    A3String data;    ///< The actual data.
    size_t   tail;    ///< The index at which to write.
    size_t   head;    ///< The index from which to read.
    size_t   max_cap; ///< The maximum allowed capacity.
} A3Buffer;

/// \brief Initialize a buffer.
///
/// If the data pointer is unset, a buffer of the given capacity will be allocated. A buffer can be
/// overlaid on existing memory by passing in a non-null data field. In such cases, `max_cap` should
/// be set to the existing capacity in order to prevent undesired reallocations, and care should be
/// taken to not call ::a3_buf_destroy or ::a3_buf_free.
A3_EXPORT bool a3_buf_init(A3Buffer*, size_t cap, size_t max_cap);

/// Allocate and initialize a buffer. See ::a3_buf_init.
A3_EXPORT A3Buffer* a3_buf_new(size_t cap, size_t max_cap);

/// Destroy a buffer, freeing the underlying data.
A3_EXPORT void a3_buf_destroy(A3Buffer*);

/// Free a buffer and destroy it.
A3_EXPORT void a3_buf_free(A3Buffer*);

/// Write a byte into the buffer.
A3_EXPORT bool a3_buf_write_byte(A3Buffer*, uint8_t);

/// Write the given string into the buffer, followed by a newline.
A3_EXPORT bool a3_buf_write_line(A3Buffer*, A3CString);

/// Write the given formatted string to the buffer. See also ::a3_buf_write_fmt.
A3_EXPORT bool a3_buf_write_vfmt(A3Buffer*, const char* fmt, va_list);

/// Write the given formatted string to the buffer.
A3_EXPORT bool a3_buf_write_fmt(A3Buffer*, const char* fmt, ...);

/// Format the given number into the buffer.
A3_EXPORT bool a3_buf_write_num(A3Buffer*, size_t);

/// Notify the buffer that bytes have been consumed.
A3_EXPORT void a3_buf_read(A3Buffer*, size_t);

/// Find the given needle string in the buffer, if it is present. Returns an A3String pointing to
/// the result (may be `NULL`).
A3_EXPORT A3String a3_buf_memmem(A3Buffer*, A3CString needle);

/// Consume the given string from the buffer. Returns `true` on success.
A3_EXPORT bool a3_buf_consume(A3Buffer*, A3CString needle);

/// See ::a3_buf_token_next.
#define A3_PRES_END_YES true

/// See ::a3_buf_token_next.
#define A3_PRES_END_NO false

/// \brief Get the next token, separated by `delim`, from the buffer.
///
/// Ending delimiters are preserved if `preserve_end` is true.
A3_EXPORT A3String a3_buf_token_next(A3Buffer*, A3CString delim, bool preserve_end);

/// \brief Same as ::a3_buf_token_next, but copies the result.
A3_ALWAYS_INLINE A3String a3_buf_token_next_copy(A3Buffer* buf, A3CString delim,
                                                 bool preserve_end) {
    return a3_string_clone(a3_buf_token_next(buf, delim, preserve_end));
}

/// Check whether the buffer has been initialized.
A3_ALWAYS_INLINE bool a3_buf_initialized(const A3Buffer* buf) {
    assert(buf);
    assert(buf->head <= buf->tail);

    return buf->data.ptr;
}

/// Clear the buffer, resetting the head and tail indices.
A3_ALWAYS_INLINE void a3_buf_reset(A3Buffer* buf) {
    assert(a3_buf_initialized(buf));

    buf->head = 0;
    buf->tail = 0;
}

/// Reset the buffer if it is empty.
A3_ALWAYS_INLINE bool a3_buf_reset_if_empty(A3Buffer* buf) {
    assert(a3_buf_initialized(buf));

    if (buf->head != buf->tail)
        return false;

    a3_buf_reset(buf);
    return true;
}

/// Get the length of the contents of the buffer.
A3_ALWAYS_INLINE size_t a3_buf_len(const A3Buffer* buf) {
    assert(a3_buf_initialized(buf));
    return buf->tail - buf->head;
}

/// Get the total available capacity for writing.
A3_ALWAYS_INLINE size_t a3_buf_cap(const A3Buffer* buf) {
    assert(a3_buf_initialized(buf));
    return buf->data.len - a3_buf_len(buf);
}

/// Get the space available for a single write (i.e., continguous space).
A3_ALWAYS_INLINE size_t a3_buf_space(A3Buffer* buf) {
    assert(a3_buf_initialized(buf));

    a3_buf_reset_if_empty(buf);
    return buf->data.len - buf->tail;
}

/// Compact the contents to the start of the buffer.
A3_ALWAYS_INLINE bool a3_buf_compact(A3Buffer* buf) {
    assert(a3_buf_initialized(buf));
    assert(buf->head != 0);

    A3_TRYB(memmove(buf->data.ptr, &buf->data.ptr[buf->head], a3_buf_len(buf)));
    buf->tail -= buf->head;
    buf->head = 0;
    return true;
}

/// Attempt to grow the buffer to fit at least min_extra_cap more bytes.
A3_ALWAYS_INLINE bool a3_buf_ensure_cap(A3Buffer* buf, size_t min_extra_cap) {
    assert(a3_buf_initialized(buf));

    if (a3_buf_space(buf) >= min_extra_cap)
        return true;
    // Nope.
    if (a3_buf_len(buf) + min_extra_cap > buf->max_cap)
        return false;

    if (a3_buf_cap(buf) >= min_extra_cap)
        return a3_buf_compact(buf);

    size_t new_cap = MAX(MAX(buf->data.len, 8), buf->max_cap);
    for (; new_cap < buf->data.len + min_extra_cap; new_cap *= 2)
        ;
    A3String new_data = a3_string_realloc(&buf->data, MIN(new_cap, buf->max_cap));
    A3_TRYB(new_data.ptr);
    buf->data = new_data;

    return true;
}

/// Attempt to grow the buffer to its maximum capacity.
A3_ALWAYS_INLINE bool a3_buf_ensure_max_cap(A3Buffer* buf) {
    assert(a3_buf_initialized(buf));

    if (buf->data.len >= buf->max_cap)
        return true;

    return a3_buf_ensure_cap(buf, buf->max_cap - buf->data.len);
}

/// Get a pointer for writing into the buffer.
A3_ALWAYS_INLINE A3String a3_buf_write_ptr(A3Buffer* buf) {
    assert(buf);

    a3_buf_reset_if_empty(buf);
#ifdef __cplusplus
    return { buf->data.ptr + buf->tail, a3_buf_space(buf) };
#else
    return (A3String) { .ptr = buf->data.ptr + buf->tail, .len = a3_buf_space(buf) };
#endif
}

/// Get a pointer for reading from the buffer.
A3_ALWAYS_INLINE A3CString a3_buf_read_ptr(const A3Buffer* buf) {
    assert(a3_buf_initialized(buf));
#ifdef __cplusplus
    return { buf->data.ptr + buf->head, a3_buf_len(buf) };
#else
    return (A3CString) { .ptr = buf->data.ptr + buf->head, .len = a3_buf_len(buf) };
#endif
}

/// Notify the buffer that bytes have been written into it.
A3_ALWAYS_INLINE void a3_buf_wrote(A3Buffer* buf, size_t len) {
    assert(a3_buf_initialized(buf));
    assert(buf->tail + len <= buf->data.len);

    buf->tail += len;
}

/// Write a string into the buffer.
A3_ALWAYS_INLINE bool a3_buf_write_str(A3Buffer* buf, A3CString str) {
    assert(a3_buf_initialized(buf));

    if (str.len + a3_buf_len(buf) > buf->max_cap)
        return false;
    A3_TRYB(a3_buf_ensure_cap(buf, str.len));

    a3_string_copy(a3_buf_write_ptr(buf), str);
    a3_buf_wrote(buf, str.len);
    return true;
}

/// Copy a struct into the buffer.
#define A3_BUF_WRITE_STRUCT(BUF, S) a3_buf_write_str((BUF), A3_CS_OBJ((S)))

A3_H_END
