/*
 * STRING -- Fat pointers for strings and byte strings.
 *
 * Copyright (c) 2020, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#include <a3/str.h>

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#ifndef _MSC_VER
#include <strings.h>
#endif

#include <a3/util.h>

String string_alloc(size_t len) {
    return (String) { .ptr = calloc(len + 1, sizeof(char)), .len = len };
}

String string_realloc(String* this, size_t new_len) {
    String ret = { .ptr = realloc(this->ptr, new_len + 1), .len = new_len };
    ret.ptr[new_len] = '\0';
    this->ptr  = NULL;
    this->len  = 0;
    return ret;
}

String string_clone(CString other) {
    if (!other.ptr)
        return S_NULL;

    String ret = string_alloc(other.len);
    string_copy(ret, other);
    return ret;
}

void string_free(String* this) {
    assert(this->ptr);
    free(this->ptr);
    this->ptr = NULL;
    this->len = 0;
}

void string_copy(String dst, CString src) {
    if (!dst.ptr || !src.ptr || dst.ptr == src.ptr)
        return;
    memcpy(dst.ptr, src.ptr, MIN(dst.len, src.len));
}

void string_concat(String str, size_t count, ...) {
    assert(str.ptr);

    va_list args;
    va_start(args, count);

    size_t offset = 0;
    for (size_t i = 0; i < count; i++) {
        CString arg = va_arg(args, CString);
        string_copy(S_OFFSET(str, offset), arg);
        offset += arg.len;
    }

    va_end(args);
}

void string_reverse(String str) {
    for (size_t i = 0; i < str.len / 2; i++) {
        uint8_t tmp              = str.ptr[i];
        str.ptr[i]               = str.ptr[str.len - 1 - i];
        str.ptr[str.len - 1 - i] = tmp;
    }
}

String string_itoa_into(String dst, size_t v) {
    size_t i = 0;
    do {
        dst.ptr[i] = (uint8_t) "0123456789"[v % 10];
        v /= 10;
        i++;
    } while (i <= dst.len && v);
    String ret = { .ptr = dst.ptr, .len = i };
    string_reverse(ret);
    return ret;
}

String string_itoa(size_t v) {
    double digits = v > 0 ? floor(log10((double)v)) + 1.0 : 1.0;
    String dst    = string_alloc((size_t)digits);
    return string_itoa_into(dst, v);
}

bool string_isascii(CString str) {
    assert(str.ptr);

    for (size_t i = 0; i < str.len; i++)
        if (!isascii(str.ptr[i]))
            return false;
    return true;
}

int string_cmp(CString s1, CString s2) {
    assert(s1.ptr && s2.ptr);
    if (s1.len != s2.len)
        return -1;
    return strncmp((char*)s1.ptr, (char*)s2.ptr, s1.len);
}

int string_cmpi(CString s1, CString s2) {
    assert(s1.ptr && s2.ptr);
    if (s1.len != s2.len)
        return -1;
    return strncasecmp((char*)s1.ptr, (char*)s2.ptr, s1.len);
}

CString string_rchr(CString str, uint8_t c) {
    assert(str.ptr);

    for (size_t i = str.len - 1;; i--) {
        if (str.ptr[i] == c)
            return (CString) { .ptr = &str.ptr[i], .len = str.len - i };

        if (i == 0)
            break;
    }

    return CS_NULL;
}
