/*
 * BUFFER -- A growable buffer.
 *
 * Copyright (c) 2020-2021, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

// For memmem.
#define _GNU_SOURCE
#define _DARWIN_C_SOURCE __DARWIN_C_FULL

#include <a3/buffer.h>

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <a3/platform/types.h>
#include <a3/platform/util.h>
#include <a3/str.h>
#include <a3/util.h>

// To my future self: This is necessary so that a symbol can be generated for
// this inline function. On debug builds, the inline hint may be ignored,
// leading to an "undefined reference" error at link time if these declarations
// are omittted. The corresponding definitions in the header are marked
// A3_EXPORT so that these symbols appear in the library.
extern inline bool      a3_buf_initialized(const A3Buffer*);
extern inline void      a3_buf_reset(A3Buffer*);
extern inline bool      a3_buf_reset_if_empty(A3Buffer*);
extern inline size_t    a3_buf_len(const A3Buffer*);
extern inline size_t    a3_buf_cap(const A3Buffer*);
extern inline size_t    a3_buf_space(A3Buffer*);
extern inline bool      a3_buf_compact(A3Buffer*);
extern inline bool      a3_buf_ensure_cap(A3Buffer*, size_t);
extern inline bool      a3_buf_ensure_max_cap(A3Buffer*);
extern inline A3String  a3_buf_write_ptr(A3Buffer*);
extern inline A3CString a3_buf_read_ptr(const A3Buffer*);
extern inline bool      a3_buf_write_str(A3Buffer*, A3CString);
extern inline void      a3_buf_wrote(A3Buffer*, size_t);

// TODO: This should probably hand out slices of a pre-registered buffer of some
// kind, to reduce the overhead of malloc and of mapping buffers into kernel
// memory.
bool a3_buf_init(A3Buffer* this, size_t cap, size_t max_cap) {
    if (!this->data.ptr)
        this->data = a3_string_alloc(cap);
    A3_TRYB(this->data.ptr);
    this->max_cap = max_cap;

    return true;
}

A3Buffer* a3_buf_new(size_t cap, size_t max_cap) {
    A3Buffer* ret = calloc(1, sizeof(A3Buffer));
    a3_buf_init(ret, cap, max_cap);
    return ret;
}

void a3_buf_destroy(A3Buffer* buf) {
    assert(a3_buf_initialized(buf));

    a3_string_free(&buf->data);
    A3_STRUCT_ZERO(buf);
}

void a3_buf_free(A3Buffer* buf) {
    assert(a3_buf_initialized(buf));
    a3_buf_destroy(buf);
    free(buf);
}

bool a3_buf_write_byte(A3Buffer* this, uint8_t byte) {
    assert(a3_buf_initialized(this));

    A3_TRYB(a3_buf_ensure_cap(this, 1));

    this->data.ptr[this->tail++] = byte;

    return true;
}

bool a3_buf_write_line(A3Buffer* this, A3CString str) {
    A3_TRYB(a3_buf_write_str(this, str));
    return a3_buf_write_byte(this, '\n');
}

A3_FORMAT_FN(2, 0)
bool a3_buf_write_vfmt(A3Buffer* this, const char* fmt, va_list args) {
    assert(a3_buf_initialized(this));
    assert(fmt);

    A3String write_ptr = a3_buf_write_ptr(this);
    int      rc        = -1;
    if ((rc = vsnprintf((char*)write_ptr.ptr, write_ptr.len, fmt, args)) < 0)
        return false;

    a3_buf_wrote(this, MIN(write_ptr.len, (size_t)rc));

    return true;
}

A3_FORMAT_FN(2, 3)
bool a3_buf_write_fmt(A3Buffer* this, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    bool ret = a3_buf_write_vfmt(this, fmt, args);
    va_end(args);

    return ret;
}

bool a3_buf_write_num(A3Buffer* this, size_t num) {
    uint8_t  tmp[20] = { '\0' };
    A3String num_str = a3_string_itoa_into((A3String) { .ptr = tmp, .len = sizeof(tmp) }, num);
    return a3_buf_write_str(this, A3_S_CONST(num_str));
}

// Bytes have been consumed from the buffer.
void a3_buf_read(A3Buffer* this, size_t len) {
    assert(a3_buf_initialized(this));
    assert(this->head + len <= this->data.len);

    this->head += len;
    a3_buf_reset_if_empty(this);
}

#ifdef _WIN32
// Windows doesn't have memmem.
static void* memmem(const void* haystack, size_t haystacklen, const void* needle,
                    size_t needlelen) {
    if (!haystack || !haystacklen || !needle || !needlelen)
        return NULL;

    for (const uint8_t* sp = haystack; sp + needlelen < (const uint8_t*)haystack + haystacklen;
         sp++) {
        if (memcmp(sp, needle, needlelen) == 0)
            return (void*)sp;
    }

    return NULL;
}
#endif

A3String a3_buf_memmem(A3Buffer* this, A3CString needle) {
    assert(a3_buf_initialized(this));
    assert(needle.ptr);
    assert(needle.len > 0);

    if (a3_buf_len(this) == 0)
        return A3_S_NULL;

    uint8_t* ret_ptr =
        memmem(&this->data.ptr[this->head], a3_buf_len(this), needle.ptr, needle.len);
    return (A3String) { .ptr = ret_ptr, .len = needle.len };
}

// Get a token from the buffer. NOTE: This updates the head of the buffer, so
// care should be taken not to write into the buffer as long as the returned
// pointer is needed.
A3String a3_buf_token_next(A3Buffer* this, A3CString delim, bool preserve_end) {
    if (!a3_buf_initialized(this))
        return A3_S_NULL;

    // <head>[delim][token][delim]...<tail>

    // Eat preceding delimiters.
    for (; this->head < this->tail && memchr(delim.ptr, this->data.ptr[this->head], delim.len);
         a3_buf_read(this, 1))
        ;

    // <head>[token][delim]...<tail>

    // Find following delimiter.
    size_t end = this->head;
    for (; end < this->tail && !memchr(delim.ptr, this->data.ptr[end], delim.len); end++)
        ;

    // Zero out all delimiters.
    size_t last = end;
    if (!preserve_end)
        for (; last < this->tail && memchr(delim.ptr, this->data.ptr[last], delim.len); last++)
            this->data.ptr[last] = '\0';

    A3String ret = { .ptr = &this->data.ptr[this->head], .len = end - this->head };
    this->head   = last;
    return ret;
}

bool a3_buf_consume(A3Buffer* this, A3CString needle) {
    assert(this);
    assert(needle.ptr);

    A3String pos = a3_buf_memmem(this, needle);
    A3_TRYB(pos.ptr);

    if ((size_t)(pos.ptr - this->data.ptr) != this->head)
        return false;

    a3_buf_read(this, needle.len);
    return true;
}
