/*
 * STRING -- Fat pointers for strings and byte strings.
 *
 * Copyright (c) 2020-2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

// For memmem.
#define _GNU_SOURCE
#define _DARWIN_C_SOURCE __DARWIN_C_FULL
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <a3/str.h>

#ifndef _MSC_VER
#include <strings.h>
#endif

#include <a3/util.h>

A3String a3_string_alloc(size_t len) {
    return (A3String) { .ptr = calloc(len + 1, sizeof(uint8_t)), .len = len };
}

A3String a3_string_realloc(A3String* this, size_t new_len) {
    A3String ret     = { .ptr = realloc(this->ptr, new_len + 1), .len = new_len };
    ret.ptr[new_len] = '\0';
    this->ptr        = NULL;
    this->len        = 0;
    return ret;
}

A3String a3_string_clone_impl(A3CString other) {
    if (!other.ptr)
        return A3_S_NULL;

    A3String ret = a3_string_alloc(other.len);
    a3_string_copy(ret, other);
    return ret;
}

void a3_string_free(A3String* this) {
    assert(this);
    assert(this->ptr);
    free(this->ptr);
    this->ptr = NULL;
    this->len = 0;
}

void a3_string_copy(A3String dst, A3CString src) {
    if (!dst.ptr || !src.ptr || dst.ptr == src.ptr)
        return;
    memcpy(dst.ptr, src.ptr, MIN(dst.len, src.len));
}

void a3_string_concat(A3String dst, size_t count, ...) {
    assert(dst.ptr);

    va_list args;
    va_start(args, count);

    size_t offset = 0;
    for (size_t i = 0; i < count && offset < dst.len; i++) {
        A3CString arg = va_arg(args, A3CString);
        a3_string_copy(a3_string_offset(dst, offset), arg);
        offset += arg.len;
    }

    va_end(args);
}

void a3_string_reverse(A3String str) {
    for (size_t i = 0; i < str.len / 2; i++) {
        uint8_t tmp              = str.ptr[i];
        str.ptr[i]               = str.ptr[str.len - 1 - i];
        str.ptr[str.len - 1 - i] = tmp;
    }
}

void a3_string_lowercase(A3String str) {
    for (size_t i = 0; i < str.len && str.ptr[i]; i++)
        str.ptr[i] = (uint8_t)tolower(str.ptr[i]);
}

A3String a3_string_to_lowercase_impl(A3CString str) {
    A3String ret = a3_string_clone(str);
    a3_string_lowercase(ret);
    return ret;
}

A3String a3_string_itoa_into(A3String dst, size_t v) {
    size_t i = 0;
    do {
        dst.ptr[i] = (uint8_t) "0123456789"[v % 10];
        v /= 10;
        i++;
    } while (i <= dst.len && v);
    A3String ret = { .ptr = dst.ptr, .len = i };
    a3_string_reverse(ret);
    return ret;
}

A3String a3_string_itoa(size_t v) {
    double   digits = v > 0 ? floor(log10((double)v)) + 1.0 : 1.0;
    A3String dst    = a3_string_alloc((size_t)digits);
    return a3_string_itoa_into(dst, v);
}

bool a3_string_isascii_impl(A3CString str) {
    assert(str.ptr);

    for (size_t i = 0; i < str.len; i++)
        if (!isascii(str.ptr[i]))
            return false;
    return true;
}

int a3_string_cmp_impl(A3CString lhs, A3CString rhs) {
    assert(lhs.ptr && rhs.ptr);
    if (lhs.len != rhs.len)
        return -1;
    return strncmp((char*)lhs.ptr, (char*)rhs.ptr, lhs.len);
}

int a3_string_cmpi_impl(A3CString lhs, A3CString rhs) {
    assert(lhs.ptr && rhs.ptr);
    if (lhs.len != rhs.len)
        return -1;
    return strncasecmp((char*)lhs.ptr, (char*)rhs.ptr, lhs.len);
}

A3CString a3_string_rchr_impl(A3CString str, uint8_t c) {
    assert(str.ptr);

    for (size_t i = str.len - 1;; i--) {
        if (str.ptr[i] == c)
            return (A3CString) { .ptr = &str.ptr[i], .len = str.len - i };

        if (i == 0)
            break;
    }

    return A3_CS_NULL;
}

#ifdef _WIN32
// Windows doesn't have memmem.
static void* memmem(void const* haystack, size_t haystacklen, void const* needle,
                    size_t needlelen) {
    if (!haystack || !haystacklen || !needle || !needlelen)
        return NULL;

    for (uint8_t const* sp = haystack; sp + needlelen < (uint8_t const*)haystack + haystacklen;
         sp++) {
        if (memcmp(sp, needle, needlelen) == 0)
            return (void*)sp;
    }

    return NULL;
}
#endif

A3CString a3_string_memmem_impl(A3CString haystack, A3CString needle) {
    assert(haystack.ptr);
    assert(needle.ptr);
    assert(needle.len > 0);

    if (haystack.len < needle.len)
        return A3_CS_NULL;

    return (A3CString) { .ptr = memmem(haystack.ptr, haystack.len, needle.ptr, needle.len),
                         .len = needle.len };
}
