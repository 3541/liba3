/*
 * STRING -- Fat pointers for strings and byte strings.
 *
 * Copyright (c) 2020-2021, Alex O'Brien <3541ax@gmail.com>
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

A3_H_BEGIN

typedef struct A3String {
    uint8_t* ptr;
    size_t   len;
} A3String;

typedef struct A3CString {
    const uint8_t* ptr;
    size_t         len;
} A3CString;

#ifdef __cplusplus
#define A3_CS(S)                                                               \
    (A3CString { reinterpret_cast<const uint8_t*>(S), sizeof(S) - 1 })
#define A3_CSS(S)                                                              \
    (A3CString { reinterpret_cast<const uint8_t*>(&S), sizeof(S) })
#define A3_CS_NULL (A3CString { nullptr, 0 })
#define A3_S_NULL  (A3String { nullptr, 0 })

A3_ALWAYS_INLINE A3String A3_CS_MUT(A3CString s) {
    return { const_cast<uint8_t*>(s.ptr), s.len };
}

A3_ALWAYS_INLINE A3CString A3_S_CONST(A3String s) { return { s.ptr, s.len }; }

A3_ALWAYS_INLINE A3String A3_S_OF(char* str) {
    if (!str)
        return A3_S_NULL;
    return { reinterpret_cast<uint8_t*>(str), strlen(str) };
}

A3_ALWAYS_INLINE A3String A3_S_OFFSET(A3String s, size_t offset) {
    return { s.ptr + offset, s.len - offset };
}

#else // __cplusplus
#define A3_CS(S)   ((A3CString) { .ptr = (uint8_t*)S, .len = (sizeof(S) - 1) })
#define A3_CSS(S)  ((A3CString) { .ptr = (void*)&S, .len = sizeof(S) })
#define A3_CS_NULL ((A3CString) { .ptr = NULL, .len = 0 })
#define A3_S_NULL  ((A3String) { .ptr = NULL, .len = 0 })

A3_ALWAYS_INLINE A3String A3_CS_MUT(A3CString s) {
    return (A3String) { .ptr = (uint8_t*)s.ptr, .len = s.len };
}

A3_ALWAYS_INLINE A3CString A3_S_CONST(A3String s) {
    return (A3CString) { .ptr = s.ptr, .len = s.len };
}

A3_ALWAYS_INLINE A3String A3_S_OF(char* str) {
    if (!str)
        return A3_S_NULL;
    return (A3String) { .ptr = (uint8_t*)str, .len = strlen(str) };
}

A3_ALWAYS_INLINE A3String A3_S_OFFSET(A3String s, size_t offset) {
    return (A3String) { .ptr = s.ptr + offset, .len = s.len - offset };
}

#endif // !__cplusplus

#define A3_S_F     "%.*s"
#define A3_S_FA(S) ((int)(S).len), ((S).ptr)

A3_ALWAYS_INLINE A3CString A3_CS_OF(const char* str) {
    return A3_S_CONST(A3_S_OF((char*)str));
}
A3_ALWAYS_INLINE const uint8_t* A3_S_END(A3CString s) { return s.ptr + s.len; }
A3_ALWAYS_INLINE const char*    A3_S_AS_C_STR(A3CString s) {
    return (const char*)s.ptr;
}

A3_ALWAYS_INLINE uint8_t* A3_S_PTR(A3String s) { return s.ptr; }
A3_ALWAYS_INLINE const uint8_t* A3_CS_PTR(A3CString s) { return s.ptr; }
A3_ALWAYS_INLINE size_t         A3_S_LEN(A3CString s) { return s.len; }

A3_EXPORT A3String a3_string_alloc(size_t len);
A3_EXPORT A3String a3_string_realloc(A3String*, size_t new_len);
A3_EXPORT A3String a3_string_clone(A3CString);
A3_EXPORT void     a3_string_free(A3String*);

A3_EXPORT void a3_string_copy(A3String dst, A3CString src);
A3_EXPORT void a3_string_concat(A3String dst, size_t count, ...);

void     a3_string_reverse(A3String);
A3String a3_string_itoa_into(A3String dst, size_t);
A3String a3_string_itoa(size_t);

A3_EXPORT bool      a3_string_isascii(A3CString);
A3_EXPORT int       a3_string_cmp(A3CString lhs, A3CString rhs);
A3_EXPORT int       a3_string_cmpi(A3CString lhs, A3CString rhs);
A3_EXPORT A3CString a3_string_rchr(A3CString, uint8_t c);

A3_H_END
