/* MIT License
 *
 * Copyright (c) 2017, Victor Cushman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef _DSTRING_H_
#define _DSTRING_H_

#include "darray.h"

// Allocate a dstring as the empty string "".
darray(char) dstr_alloc_empty();

// Allocate a dstring as copy of cstring `src`.
darray(char) dstr_alloc_from_cstr(const char* src);

// Allocate a dstring as a copy of dstring `src`. Faster than
// `dstr_alloc_from_cstr` for allocating a dstring from another dstring as,
// the copy is performed with one large `memcpy` instead of a char-by-char copy.
darray(char) dstr_alloc_from_dstr(const darray(char) src);

// Allocate a dstring using `sprintf` style formatting.
darray(char) dstr_alloc_from_format(const char* format, ...);

// Free a dstring. Equivalent to calling `da_free` on `dstr`.
void dstr_free(darray(char) dstr);

// Returns the length of `dstr` without it's null terminator. Equivalent to
// `da_length(dstr)-1`.
size_t dstr_length(const darray(char) dstr);

// Append `src` to dstring `dest`. Like `da_cat` references to `dest` may be
// invalidated across the function call. Use the return value as truth for the
// location of `dest` after function completion.
darray(char) dstr_cat_cstr(darray(char) dest, const char* src);

// Append `src` to dstring `dest`. Like `da_cat` references to `dest` may be
// invalidated across the function call. Use the return value as truth for the
// location of `dest` after function completion. Faster than
// `dstr_cat_cstr` for cat-ing a dstring to another dstring.
darray(char) dstr_cat_dstr(darray(char) dest, const darray(char) src);

// Comparison function. Currently functionally equivalent to `strcmp`.
int dstr_cmp(const darray(char) s1, const char* s2);

// Comparison function. Currently functionally equivalent to `strcasecmp`.
int dstr_cmp_case(const darray(char) s1, const char* s2);

// Returns the index of the first occurrence of `substr` in `dstr`. Returns -1
// if `substr` was not found. Similar to python's `str.find`.
long dstr_find(darray(char) dstr, const char* substr);

// Returns the index of the first case insensitive occurrence of `substr` in
// `dstr`. Returns -1 if `substr` was not found. Similar to python's `str.find`.
long dstr_find_case(darray(char) dstr, const char* substr);

// Replace all occurrences of `substr` in `dstr` with `new_substr`. Returns the
// new location of `dstr` after replacement.
darray(char) dstr_replace_all(darray(char) dstr, const char* substr,
    const char* new_substr);

// Replace all occurrences of `substr` (case insensitive) in `dstr` with
// `new_substr`. Returns the new location of `dstr` after replacement.
darray(char) dstr_replace_all_case(darray(char) dstr, const char* substr,
    const char* new_substr);

// Transform `dstr` to lower case in place.
void dstr_transform_lower(darray(char) dstr);

// Transform `dstr` to upper case in place.
void dstr_transform_upper(darray(char) dstr);

// Trim's leading and trailing whitespace from dstr. Returns the new location
// of `dstr` after trimming.
darray(char) dstr_trim(darray(char) dstr);

#endif // !_DSTRING_H_
