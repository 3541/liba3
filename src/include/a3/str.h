/*
 * STRING -- Fat pointers for strings and byte strings.
 *
 * Copyright (c) 2020-2021, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

/// \file str.h
/// Byte strings using fat pointers.

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <a3/cpp.h>
#include <a3/types.h>

A3_H_BEGIN

/// A byte string.
typedef struct A3String {
    uint8_t* ptr; ///< The actual pointer.
    size_t   len; ///< The string's length.
} A3String;

/// A constant byte string.
typedef struct A3CString {
    uint8_t const* ptr; ///< The actual pointer.
    size_t         len; ///< The string's length.
} A3CString;

A3_ALWAYS_INLINE A3_CONSTEXPR A3String a3_string_new(uint8_t* ptr, size_t len) {
    A3String ret A3_EMPTY_INIT;
    ret.ptr = ptr;
    ret.len = len;
    return ret;
}

A3_ALWAYS_INLINE A3_CONSTEXPR A3CString a3_cstring_new(uint8_t const* ptr, size_t len) {
    A3CString ret A3_EMPTY_INIT;
    ret.ptr = ptr;
    ret.len = len;
    return ret;
}

/// Cast a constant to a mutable one.
A3_ALWAYS_INLINE A3String A3_CS_MUT(A3CString s) { return a3_string_new((uint8_t*)s.ptr, s.len); }

#ifdef __cplusplus

A3_H_END

#define A3_CS(S)     (A3CString { reinterpret_cast<uint8_t const*>(&(S)[0]), sizeof(S) - 1 })
#define A3_CS_OBJ(S) (A3CString { reinterpret_cast<uint8_t const*>(&(S)), sizeof(S) })
#define A3_S_NULL    (A3String { nullptr, 0 })
#define A3_CS_NULL   (A3CString { nullptr, 0 })

A3_ALWAYS_INLINE A3CString A3_S_CONST(A3String s) { return a3_cstring_new(s.ptr, s.len); }
A3_ALWAYS_INLINE A3CString A3_S_CONST(A3CString s) { return s; }

A3_H_BEGIN

#else

/// Create a constant string from a literal C string.
#define A3_CS(S)     ((A3CString) { .ptr = (uint8_t const*)(S), .len = sizeof(S) - 1 })

/// Create an A3CString pointing to a struct.
#define A3_CS_OBJ(S) ((A3CString) { .ptr = (uint8_t const*)&(S), .len = sizeof(S) })

/// A null string.
#define A3_S_NULL    ((A3String) { .ptr = NULL, .len = 0 })

/// A null constant string.
#define A3_CS_NULL   ((A3CString) { .ptr = NULL, .len = 0 })

#endif

#ifndef __cplusplus
#if defined(__GNUC__) || defined(__clang__)

/// Cast a string to a constant string.
#define A3_S_CONST(X)                                                                              \
    ({                                                                                             \
        __typeof__((X)) _in_str = (X);                                                             \
        A3CString       _ret    = { .ptr = _in_str.ptr, .len = _in_str.len };                      \
        _ret;                                                                                      \
    })

#elif __STDC_VERSION__ >= 201112L

A3_ALWAYS_INLINE A3CString _A3_S_CONST(A3String s) { return a3_cstring_new(s.ptr, s.len); }
A3_ALWAYS_INLINE A3CString _A3_S_NOP(A3CString s) { return s; }
/// Cast a string to a constant string.
#define A3_S_CONST(X) _Generic((X), A3String : _A3_S_CONST, A3CString : _A3_S_NOP)(X)

#else

#error                                                                                             \
    "An overload feature of some kind (C++ overloads, compound statements, or _Generic) is required."

#endif
#endif

/// Create a string from a null-terminated `char*` (warning: uses `strlen`).
A3_ALWAYS_INLINE A3String a3_string_from(char* str) {
    if (!str)
        return A3_S_NULL;
    return a3_string_new((uint8_t*)str, strlen(str));
}

/// Create a constant string from a null-terminated `const char*` (warning: uses `strlen`).
A3_ALWAYS_INLINE A3CString a3_cstring_from(char const* str) {
    return A3_S_CONST(a3_string_from((char*)str));
}

/// Get a string at the given offset into the given string.
A3_ALWAYS_INLINE A3String a3_string_offset(A3String s, size_t offset) {
    return a3_string_new(s.ptr + offset, s.len - offset);
}

/// The format specifier to use to print a string. See ::A3_S_FORMAT.
#define A3_S_F "%.*s"

/// \brief The format argument to use to print a string.
///
/// For example:
///
///     printf("A string: " S_F ".\n", S_FA(a_string));
#define A3_S_FORMAT(S) ((int)(S).len), ((S).ptr)

/// Get a pointer one byte past the final byte of the given string.
A3_ALWAYS_INLINE uint8_t const* a3_string_end(A3CString s) { return s.ptr + s.len; }

/// Get a const string as a `const char*`.
A3_ALWAYS_INLINE char const* a3_string_cstr(A3CString s) { return (char const*)s.ptr; }

/// Get a pointer to the start of a string.
A3_ALWAYS_INLINE uint8_t* a3_string_ptr(A3String s) { return s.ptr; }

/// Get a `const` pointer to the start of a string.
A3_ALWAYS_INLINE uint8_t const* a3_string_cptr(A3CString s) { return s.ptr; }

/// Get the length of a string.
A3_ALWAYS_INLINE size_t a3_string_len(A3CString s) { return s.len; }

/// Allocate a new string of the given size.
A3_EXPORT A3String a3_string_alloc(size_t len);

/// Reallocate a string to a new size.
A3_EXPORT A3String a3_string_realloc(A3String*, size_t new_len);

/// Free a string.
A3_EXPORT void a3_string_free(A3String*);

/// \brief Copy a string from `src` to `dst`.
///
/// Will copy either the size of `src` or of `dst`, whichever is less.
A3_EXPORT void a3_string_copy(A3String dst, A3CString src);

/// \brief Concatenate `count` strings into `dst`.
///
/// Expects `count` string-type arguments. Will only copy as much as there is space for in `dst`.
A3_EXPORT void a3_string_concat(A3String dst, size_t count, ...);

/// Reverse the given string in-place.
A3_EXPORT void a3_string_reverse(A3String);

/// Convert the given string to lowercase in-place.
A3_EXPORT void a3_string_lowercase(A3String);

/// Print the given number into the given string.
A3_EXPORT A3String a3_string_itoa_into(A3String dst, size_t);

/// Print the given number to a new string.
A3_EXPORT A3String a3_string_itoa(size_t);

#ifndef DOXYGEN
// A little bit of effort is required for functions which are generic over
// String/CString. First, declare the actual implementations.
A3_EXPORT A3String a3_string_clone_impl(A3CString);

A3_EXPORT A3String a3_string_to_lowercase_impl(A3CString);

A3_EXPORT bool      a3_string_isascii_impl(A3CString);
A3_EXPORT int       a3_string_cmp_impl(A3CString lhs, A3CString rhs);
A3_EXPORT int       a3_string_cmpi_impl(A3CString lhs, A3CString rhs);
A3_EXPORT A3CString a3_string_rchr_impl(A3CString, uint8_t c);
#endif

// Then, create overloaded macros using the type-generic A3_S_CONST.

///
///     A3String a3_string_clone(A3[C]String);
///
/// \brief Clone a string.
#define a3_string_clone(S) a3_string_clone_impl(A3_S_CONST(S))

///
///     A3String a3_string_to_lowercase(A3[C]String);
///
/// \brief Clone a string and convert it to lowercase.
#define a3_string_to_lowercase(S) a3_string_to_lowercase_impl(A3_S_CONST(S))

///
///     bool a3_string_isascii(A3[C]String);
///
/// \brief Check whether a string is entirely composed of ASCII bytes.
#define a3_string_isascii(S) a3_string_isascii_impl(A3_S_CONST(S))

///
///     int a3_string_cmp(A3[C]String lhs, A3[C]String rhs);
///
/// \brief Compare two strings.
///
/// Returns a value less than, equal to, or greater than zero depending on
/// whether `lhs` is less than, equal to, or greater than `rhs`, respectively.
#define a3_string_cmp(L, R) a3_string_cmp_impl(A3_S_CONST(L), A3_S_CONST(R))

///
///     int a3_string_cmpi(A3[C]String lhs, A3[C]String rhs);
///
/// \brief Compare two strings, ignoring case. See ::a3_string_cmp.
#define a3_string_cmpi(L, R) a3_string_cmpi_impl(A3_S_CONST(L), A3_S_CONST(R))

///
///     A3CString a3_string_rchr(A3[C]String, uint8_t c);
///
/// \brief Find the given byte in the given string, if present.
#define a3_string_rchr(S, C) a3_string_rchr_impl(A3_S_CONST(S), (C))

A3_H_END
