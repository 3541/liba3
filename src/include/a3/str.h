/*
 * STRING -- Fat pointers for strings and byte strings.
 *
 * Copyright (c) 2020, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 *
 * NOTES
 * - Care should be taken not to assume that these are null-terminated.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <a3/cpp.h>
#include <a3/util.h>

H_BEGIN

typedef struct String {
    uint8_t* ptr;
    size_t   len;
} String;

typedef struct CString {
    const uint8_t* ptr;
    size_t         len;
} CString;

#define CS(S)                                                                  \
    (CString) { .ptr = (uint8_t*)S, .len = (sizeof(S) - 1) }
#define CS_NULL (CString) { .ptr = NULL, .len = 0 };
#define S_NULL  (String) { .ptr = NULL, .len = 0 };
ALWAYS_INLINE String CS_MUT(CString s) {
    return (String) { .ptr = (uint8_t*)s.ptr, .len = s.len };
}
ALWAYS_INLINE CString S_CONST(String s) {
    return (CString) { .ptr = s.ptr, .len = s.len };
}
ALWAYS_INLINE String S_OF(char* str) {
    if (!str)
        return S_NULL;
    return (String) { .ptr = (uint8_t*)str, .len = strlen(str) };
}
ALWAYS_INLINE CString CS_OF(const char* str) {
    return S_CONST(S_OF((char*)str));
}
ALWAYS_INLINE String S_OFFSET(String s, size_t offset) {
    return (String) { .ptr = s.ptr + offset, .len = s.len - offset };
}
ALWAYS_INLINE const uint8_t* S_END(CString s) { return s.ptr + s.len; }

EXPORT String string_alloc(size_t len);
EXPORT String string_realloc(String*, size_t new_len);
EXPORT String string_clone(CString);
EXPORT void   string_free(String*);

EXPORT void string_copy(String dst, CString src);
EXPORT void string_concat(String dst, size_t count, ...);

void   string_reverse(String);
String string_itoa(String dst, size_t);

EXPORT bool    string_isascii(CString);
EXPORT int     string_cmpi(CString lhs, CString rhs);
EXPORT CString string_rchr(CString, uint8_t c);

H_END
