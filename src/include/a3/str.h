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

A3_H_END

#ifdef __cplusplus
#define A3_CS(S)   (A3CString { reinterpret_cast<const uint8_t*>(S), sizeof(S) - 1 })
#define A3_CSS(S)  (A3CString { reinterpret_cast<const uint8_t*>(&S), sizeof(S) })
#define A3_CS_NULL (A3CString { nullptr, 0 })
#define A3_S_NULL  (A3String { nullptr, 0 })

A3_ALWAYS_INLINE A3String A3_CS_MUT(A3CString s) { return { const_cast<uint8_t*>(s.ptr), s.len }; }

A3_ALWAYS_INLINE A3CString A3_S_CONST(A3String s) { return { s.ptr, s.len }; }
A3_ALWAYS_INLINE A3CString A3_S_CONST(A3CString s) { return s; }

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

#ifdef a3_HAVE__Generic

A3_ALWAYS_INLINE A3CString _A3_S_CONST(A3String s) {
    return (A3CString) { .ptr = s.ptr, .len = s.len };
}
A3_ALWAYS_INLINE A3CString _A3_S_NOP(A3CString s) { return s; }
#define A3_S_CONST(X) _Generic((X), A3String : _A3_S_CONST, A3CString : _A3_S_NOP)(X)

#else

#define A3_S_CONST(X)                                                                              \
    ({                                                                                             \
        __typeof__((X)) _in_str = (X);                                                             \
        A3CString _ret          = { .ptr = _in_str.ptr, .len = _in_str.len };                      \
        _ret;                                                                                      \
    })

#endif

A3_ALWAYS_INLINE A3String A3_S_OF(char* str) {
    if (!str)
        return A3_S_NULL;
    return (A3String) { .ptr = (uint8_t*)str, .len = strlen(str) };
}

A3_ALWAYS_INLINE A3String A3_S_OFFSET(A3String s, size_t offset) {
    return (A3String) { .ptr = s.ptr + offset, .len = s.len - offset };
}

#endif // !__cplusplus

A3_H_BEGIN

#define A3_S_F     "%.*s"
#define A3_S_FA(S) ((int)(S).len), ((S).ptr)

A3_ALWAYS_INLINE A3CString A3_CS_OF(const char* str) { return A3_S_CONST(A3_S_OF((char*)str)); }
A3_ALWAYS_INLINE const uint8_t* A3_S_END(A3CString s) { return s.ptr + s.len; }
A3_ALWAYS_INLINE const char*    A3_S_AS_C_STR(A3CString s) { return (const char*)s.ptr; }

A3_ALWAYS_INLINE uint8_t* A3_S_PTR(A3String s) { return s.ptr; }
A3_ALWAYS_INLINE const uint8_t* A3_CS_PTR(A3CString s) { return s.ptr; }
A3_ALWAYS_INLINE size_t         A3_S_LEN(A3CString s) { return s.len; }

A3_EXPORT A3String a3_string_alloc(size_t len);
A3_EXPORT A3String a3_string_realloc(A3String*, size_t new_len);
A3_EXPORT void     a3_string_free(A3String*);

A3_EXPORT void a3_string_copy(A3String dst, A3CString src);
A3_EXPORT void a3_string_concat(A3String dst, size_t count, ...);

A3_EXPORT void     a3_string_reverse(A3String);
A3_EXPORT void     a3_string_lowercase(A3String);
A3_EXPORT A3String a3_string_itoa_into(A3String dst, size_t);
A3_EXPORT A3String a3_string_itoa(size_t);

// A little bit of effort is required for functions which are generic over
// String/CString. First, declare the actual implementations.
A3_EXPORT A3String a3_string_clone_impl(A3CString);

A3_EXPORT A3String a3_string_to_lowercase_impl(A3CString);

A3_EXPORT bool      a3_string_isascii_impl(A3CString);
A3_EXPORT int       a3_string_cmp_impl(A3CString lhs, A3CString rhs);
A3_EXPORT int       a3_string_cmpi_impl(A3CString lhs, A3CString rhs);
A3_EXPORT A3CString a3_string_rchr_impl(A3CString, uint8_t c);

// Then, create overloaded macros using the type-generic A3_S_CONST.

#define a3_string_clone(S) a3_string_clone_impl(A3_S_CONST(S))

#define a3_string_to_lowercase(S) a3_string_to_lowercase_impl(A3_S_CONST(S))

#define a3_string_isascii(S) a3_string_isascii_impl(A3_S_CONST(S))
#define a3_string_cmp(L, R)  a3_string_cmp_impl(A3_S_CONST(L), A3_S_CONST(R))
#define a3_string_cmpi(L, R) a3_string_cmpi_impl(A3_S_CONST(L), A3_S_CONST(R))
#define a3_string_rchr(S, C) a3_string_rchr_impl(A3_S_CONST(S), (C))

A3_H_END
