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
#include <stdarg.h>
#include <stdio.h>

/**@function
 * @brief Allocate a dstring as the empty string `""`.
 *
 * @return Pointer to a new dstring on success. `NULL` on allocation failure.
 */
darray(char) dstr_alloc_empty(void) DA_WARN_UNUSED_RESULT;

/**@function
 * @brief Allocate a dstring as copy of cstring `src`. `src` may also be a
 *  dstring.
 *
 * @param src : string to copy.
 *
 * @return Pointer to a new dstring on success. `NULL` on allocation failure.
 */
darray(char) dstr_alloc_from_cstr(const char* src) DA_WARN_UNUSED_RESULT;

/**@function
 * @brief Allocate a dstring as a copy of dstring `src`. Faster than
 *  `dstr_alloc_from_cstr` when copying a dstring.
 *
 * @param src : dstring to copy.
 *
 * @return Pointer to a new dstring on success. `NULL` on allocation failure.
 */
darray(char) dstr_alloc_from_dstr(const darray(char) src) DA_WARN_UNUSED_RESULT;

/**@function
 * @brief Allocate a dstring using `sprintf` style formatting.
 *
 * @param format : `sprintf` style format string.
 * @param ... : va arg list for the format string.
 *
 * @return Pointer to a new dstring on success. `NULL` on allocation failure.
 */
darray(char) dstr_alloc_from_format(const char* format, ...)
    DA_WARN_UNUSED_RESULT;

/**@function
 * @brief Free a dstring. Equivalent to calling `da_free` on `dstr`.
 * 
 * @param dstr : dstring to free.
 */
void dstr_free(darray(char) dstr);

/**@function
 * @brief Reassign the contents of an allocated dstring to the empty string
 *  `""`, reallocating memory only when neccesary.
 *
 * @param allocated_dstr : Dstring containing memory allocated by one of the
 *  dstr_alloc functions.
 *
 * @return Pointer to the reassigned dstring on success. `NULL` on allocation
 *  failure.
 */
darray(char) dstr_reassign_empty(darray(char) allocated_dstr)
    DA_WARN_UNUSED_RESULT;

/**@function
 * @brief Reassign the contents of an allocated dstring to that of a cstring,
 *  reallocating memory only when neccesary.
 *
 * @param allocated_dstr : Dstring containing memory allocated by one of the
 *  dstr_alloc functions.
 * @param src : string to copy.
 *
 * @return Pointer to the reassigned dstring on success. `NULL` on allocation
 *  failure.
 */
darray(char) dstr_reassign_from_cstr(darray(char) allocated_dstr,
    const char* src) DA_WARN_UNUSED_RESULT;


/**@function
 * @brief Reassign the contents of an allocated dstring to that of a dstring,
 *  reallocating memory only when neccesary.
 *
 * @param allocated_dstr : Dstring containing memory allocated by one of the
 *  dstr_alloc functions.
 * @param src : dstring to copy.
 *
 * @return Pointer to the reassigned dstring on success. `NULL` on allocation
 *  failure.
 */
darray(char) dstr_reassign_from_dstr(darray(char) allocated_dstr,
    const darray(char) src) DA_WARN_UNUSED_RESULT;

/**@function
 * @brief Reassign the contents of an allocated dstring using `sprintf` style
 *  formatting, reallocating memory only when neccesary.
 *
 * @param format : `sprintf` style format string.
 * @param ... : va arg list for the format string.
 *
 * @return Pointer to the reassigned dstring on success. `NULL` on allocation
 *  failure.
 */
darray(char) dstr_reassign_from_format(darray(char) allocated_dstr,
    const char* format, ...) DA_WARN_UNUSED_RESULT;

/**@function
 * @brief Returns the length of `dstr` without its null terminator. Equivalent
 *  to `da_length(dstr)-1`. O(1) compared to the O(n) `strlen`.
 *
 * @param dstr : Target dstring.
 *
 * @return Length of `dstr` without its null terminator.
 */
size_t dstr_length(const darray(char) dstr);

/**@function
 * @brief Append `src` to dstring `dest`. `src` may also be a dstring.
 *
 * @param dest : Target dstring that will be appended to. Like `da_concat`
 *  references to `dest` may be invalidated across the function call. Use the
 *  return value of `dstr_concat_cstr` as truth for the location of `dest` after
 *  function completion.
 * @param src : cstring to append to `dest`.
 *
 * @return Pointer to the new location of the dstring upon successful function
 *  completion. If `dstr_concat_cstr` returns `NULL`, reallocation failed and
 *  `dstr` is left untouched.
 */
darray(char) dstr_concat_cstr(darray(char) dest, const char* src)
    DA_WARN_UNUSED_RESULT;

/**@function
 * @brief Append `src` to dstring `dest`. Faster than `dstr_concat_cstr` for
 *  cat-ing a dstring to another dstring.
 * 
 * @param dest : Target dstring that will be appended to. Like `da_concat`
 *  references to `dest` may be invalidated across the function call. Use the
 *  return value of `dstr_concat_cstr` as truth for the location of `dest` after
 *  function completion.
 * @param src : dstring to append to `dest`.
 */
darray(char) dstr_concat_dstr(darray(char) dest, const darray(char) src)
    DA_WARN_UNUSED_RESULT;

/**@function
 * @brief Comparison function. Currently functionally equivalent to `strcmp`.
 *
 * @param s1 : First dstring.
 * @param s2 : Second dstring.
 *
 * @return `strcmp` style comparison of `s1` and `s2`.
 */
int dstr_cmp(const darray(char) s1, const char* s2);

/**@function
 * @brief Comparison function. Currently functionally equivalent to
 *  `strcasecmp`.
 *
 * @param s1 : First dstring.
 * @param s2 : Second dstring.
 *
 * @return `strcmp` style comparison of `s1` and `s2`.
 */
int dstr_cmp_case(const darray(char) s1, const char* s2);

/**@function
 * @brief Returns the index of the first occurrence of `substr` in `dstr` or
 *  -1 if `substr` was not found. Similar to Python's `str.find`.
 *
 * @param dstr : Target dstring to search.
 * @param substr : Target substring to find in `dstr`.
 *
 * @return Index of the first occurence of `substr` if `substr` was found. `-1`
 *  if  substr was not found.
 */
long dstr_find(darray(char) dstr, const char* substr);

/**@function
 * @brief Returns the index of the first case insensitive occurrence of
 *  `substr` in `dstr` or -1 if `substr` was not found. Similar to Python's
 *  `str.find`.
 *
 * @param dstr : Target dstring to search.
 * @param substr : Target substring to find in `dstr`.
 *
 * @return Index of the first occurence of `substr` if `substr` was found. `-1`
 *  if substr was not found.
 */
long dstr_find_case(darray(char) dstr, const char* substr);

/**@function
 * @brief Replace all occurrences of `substr` in `dstr` with `new_str`.
 *
 * @param dstr : Target dstring. Upon function completion, `dstr` may or may not
 *  point to its previous block on the heap, potentially breaking references.
 * @param substr : Substring in `dstr` that will be replaces.
 * @param new_substr : String that will replace `substr`.
 *
 * @return The new location of `dstr` after function completion. If
 *  `dstr_replace_all` returns `NULL` reallocation failed and `dstr` is left
 *  untouched.
 */
darray(char) dstr_replace_all(darray(char) dstr, const char* substr,
    const char* new_str) DA_WARN_UNUSED_RESULT;

/**@function
 * @brief Replace all occurrences of `substr` (case insensitive) in `dstr` with
 *  `new_str`.
 *
 * @param dstr : Target dstring. Upon function completion, `dstr` may or may not
 *  point to its previous block on the heap, potentially breaking references.
 * @param substr : Substring in `dstr` that will be replaces.
 * @param new_substr : String that will replace `substr`.
 *
 * @return The new location of `dstr` after function completion. If
 *  `dstr_replace_all_case` returns `NULL` reallocation failed and `dstr` is
 *  left untouched.
 */
darray(char) dstr_replace_all_case(darray(char) dstr, const char* substr,
    const char* new_str) DA_WARN_UNUSED_RESULT;

/**@function
 * @brief Transform `dstr` to lower case in place.
 *
 * @param dstr : Target dstring to transform.
 */
void dstr_transform_lower(darray(char) dstr);

/**@function
 * @brief Transform `dstr` to upper case in place.
 *
 * @param dstr : Target dstring to transform.
 */
void dstr_transform_upper(darray(char) dstr);

/**@function
 * @brief Trims leading and trailing whitespace from `dstr`.
 *
 * @param : Target dstring. Upon function completion, `dstr` may or may not
 *  point to its previous block on the heap, potentially breaking references.
 *
 * @return The new location of `dstr` after function completion. If `dstr_trim`
 *  returns `NULL` reallocation failed and `dstr` is left untouched.
 */
darray(char) dstr_trim(darray(char) dstr) DA_WARN_UNUSED_RESULT;

#endif // !_DSTRING_H_
