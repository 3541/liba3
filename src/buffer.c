/*
 * BUFFER -- A growable buffer.
 *
 * Copyright (c) 2020, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

// For memmem.
#define _GNU_SOURCE

#include <a3/buffer.h>

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <a3/platform.h>
#include <a3/str.h>
#include <a3/util.h>

// To my future self: This is necessary so that a symbol can be generated for
// this inline function. On debug builds, the inline hint may be ignored,
// leading to an "undefined reference" error at link time if these declarations
// are omittted. The corresponding definitions in the header are marked EXPORT
// so that these symbols appear in the library.
extern inline bool    buf_initialized(const Buffer*);
extern inline void    buf_reset(Buffer*);
extern inline bool    buf_reset_if_empty(Buffer*);
extern inline size_t  buf_len(const Buffer*);
extern inline size_t  buf_cap(const Buffer*);
extern inline size_t  buf_space(Buffer*);
extern inline bool    buf_compact(Buffer*);
extern inline bool    buf_ensure_cap(Buffer*, size_t);
extern inline bool    buf_ensure_max_cap(Buffer*);
extern inline String  buf_write_ptr(Buffer*);
extern inline CString buf_read_ptr(const Buffer*);
extern inline bool    buf_write_str(Buffer*, CString);
extern inline void    buf_wrote(Buffer*, size_t);

// TODO: This should probably hand out slices of a pre-registered buffer of some
// kind, to reduce the overhead of malloc and of mapping buffers into kernel
// memory. For now, it just allocates or initializes a buffer. A buffer can be
// overlaid on existing memory by passing it to buf_init with a non-null data
// field. In such cases, care should be taken not to trigger an unintended
// resize (and thus copy). This can be simplified by initializing with cap ==
// max_cap.
bool buf_init(Buffer* this, size_t cap, size_t max_cap) {
    if (!this->data.ptr)
        this->data = string_alloc(cap);
    TRYB(this->data.ptr);
    this->max_cap = max_cap;

    return true;
}

Buffer* buf_new(size_t cap, size_t max_cap) {
    Buffer* ret = calloc(1, sizeof(Buffer));
    buf_init(ret, cap, max_cap);
    return ret;
}

void buf_destroy(Buffer* buf) {
    assert(buf_initialized(buf));

    string_free(&buf->data);
    memset(buf, 0, sizeof(Buffer));
}

void buf_free(Buffer* buf) {
    assert(buf_initialized(buf));
    buf_destroy(buf);
    free(buf);
}

bool buf_write_byte(Buffer* this, uint8_t byte) {
    assert(buf_initialized(this));

    TRYB(buf_ensure_cap(this, 1));

    this->data.ptr[this->tail++] = byte;

    return true;
}

bool buf_write_line(Buffer* this, CString str) {
    TRYB(buf_write_str(this, str));
    return buf_write_byte(this, '\n');
}

FORMAT_FN(2, 0)
bool buf_write_vfmt(Buffer* this, const char* fmt, va_list args) {
    assert(buf_initialized(this));
    assert(fmt);

    String write_ptr = buf_write_ptr(this);
    int    rc        = -1;
    if ((rc = vsnprintf((char*)write_ptr.ptr, write_ptr.len, fmt, args)) < 0)
        return false;

    buf_wrote(this, MIN(write_ptr.len, (size_t)rc));

    return true;
}

FORMAT_FN(2, 3)
bool buf_write_fmt(Buffer* this, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    bool ret = buf_write_vfmt(this, fmt, args);
    va_end(args);

    return ret;
}

bool buf_write_num(Buffer* this, size_t num) {
    static THREAD_LOCAL uint8_t _BUF[20] = { '\0' };
    String                      num_str =
        string_itoa_into((String) { .ptr = _BUF, .len = sizeof(_BUF) }, num);
    return buf_write_str(this, S_CONST(num_str));
}

// Bytes have been consumed from the buffer.
void buf_read(Buffer* this, size_t len) {
    assert(buf_initialized(this));
    assert(this->head + len <= this->data.len);

    this->head += len;
    buf_reset_if_empty(this);
}

#ifdef _WIN32
// Windows doesn't have memmem.
static void* memmem(const void* haystack, size_t haystacklen,
                    const void* needle, size_t needlelen) {
    if (!haystack || !haystacklen || !needle || !needlelen)
        return NULL;

    for (const uint8_t* sp = haystack;
         sp + needlelen < (const uint8_t*)haystack + haystacklen; sp++) {
        if (memcmp(sp, needle, needlelen) == 0)
            return (void*)sp;
    }

    return NULL;
}
#endif

String buf_memmem(Buffer* this, CString needle) {
    assert(buf_initialized(this));
    assert(needle.ptr);
    assert(needle.len > 0);

    if (buf_len(this) == 0)
        return S_NULL;

    uint8_t* ret_ptr = memmem(&this->data.ptr[this->head], buf_len(this),
                              needle.ptr, needle.len);
    return (String) { .ptr = ret_ptr, .len = needle.len };
}

// Get a token from the buffer. NOTE: This updates the head of the buffer, so
// care should be taken not to write into the buffer as long as the returned
// pointer is needed.
String buf_token_next_impl(_buf_token_next_args args) {
    struct Buffer* this  = args.buf;
    CString delim        = args.delim;
    bool    preserve_end = args.preserve_end;
    assert(buf_initialized(this));

    // <head>[delim][token][delim]...<tail>

    // Eat preceding delimiters.
    for (; this->head < this->tail &&
           memchr(delim.ptr, this->data.ptr[this->head], delim.len);
         buf_read(this, 1))
        ;

    // <head>[token][delim]...<tail>

    // Find following delimiter.
    size_t end = this->head;
    for (;
         end < this->tail && !memchr(delim.ptr, this->data.ptr[end], delim.len);
         end++)
        ;

    // Zero out all delimiters.
    size_t last = end;
    if (!preserve_end)
        for (; last < this->tail &&
               memchr(delim.ptr, this->data.ptr[last], delim.len);
             last++)
            this->data.ptr[last] = '\0';

    String ret = { .ptr = &this->data.ptr[this->head],
                   .len = end - this->head };
    this->head = last;
    return ret;
}

bool buf_consume(Buffer* this, CString needle) {
    assert(this);
    assert(needle.ptr);

    String pos = buf_memmem(this, needle);
    TRYB(pos.ptr);

    if ((size_t)(pos.ptr - this->data.ptr) != this->head)
        return false;

    buf_read(this, needle.len);
    return true;
}
