/*
 * STRING -- Fat pointers for strings and byte strings.
 *
 * Copyright (c) 2020, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <a3/cpp.h>
#include <a3/platform/types.h>

H_BEGIN

typedef struct String {
    uint8_t* ptr;
    size_t   len;
} String;

typedef struct CString {
    const uint8_t* ptr;
    size_t         len;
} CString;

#ifdef __cplusplus
#define CS(S)   (CString { reinterpret_cast<const uint8_t*>(S), sizeof(S) - 1 })
#define CS_NULL (CString { nullptr, 0 })
#define S_NULL  (String { nullptr, 0 })

ALWAYS_INLINE String CS_MUT(CString s) {
    return { const_cast<uint8_t*>(s.ptr), s.len };
}

ALWAYS_INLINE CString S_CONST(String s) { return { s.ptr, s.len }; }

ALWAYS_INLINE String  S_OF(char* str) {
    if (!str)
        return S_NULL;
    return { reinterpret_cast<uint8_t*>(str), strlen(str) };
}

ALWAYS_INLINE String S_OFFSET(String s, size_t offset) {
    return { s.ptr + offset, s.len - offset };
}

#else // __cplusplus
#define CS(S)   ((CString) { .ptr = (uint8_t*)S, .len = (sizeof(S) - 1) })
#define CS_NULL ((CString) { .ptr = NULL, .len = 0 })
#define S_NULL  ((String) { .ptr = NULL, .len = 0 })

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

ALWAYS_INLINE String S_OFFSET(String s, size_t offset) {
    return (String) { .ptr = s.ptr + offset, .len = s.len - offset };
}

#endif // !__cplusplus

#define S_F "%.*s"
#define S_FA(S) ((int)(S).len), ((S).ptr)

ALWAYS_INLINE CString CS_OF(const char* str) {
    return S_CONST(S_OF((char*)str));
}
ALWAYS_INLINE const uint8_t* S_END(CString s) { return s.ptr + s.len; }
ALWAYS_INLINE const char* S_AS_C_STR(CString s) { return (const char*)s.ptr; }

ALWAYS_INLINE uint8_t* S_PTR(String s) { return s.ptr; }
ALWAYS_INLINE const uint8_t* CS_PTR(CString s) { return s.ptr; }
ALWAYS_INLINE size_t         S_LEN(CString s) { return s.len; }

EXPORT String string_alloc(size_t len);
EXPORT String string_realloc(String*, size_t new_len);
EXPORT String string_clone(CString);
EXPORT void   string_free(String*);

EXPORT void string_copy(String dst, CString src);
EXPORT void string_concat(String dst, size_t count, ...);

void   string_reverse(String);
String string_itoa_into(String dst, size_t);
String string_itoa(size_t);

EXPORT bool    string_isascii(CString);
EXPORT int     string_cmp(CString lhs, CString rhs);
EXPORT int     string_cmpi(CString lhs, CString rhs);
EXPORT CString string_rchr(CString, uint8_t c);

H_END
