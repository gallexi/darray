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

darray(char) dstr_alloc_empty();
darray(char) dstr_alloc_from_cstr(const char* src);
darray(char) dstr_alloc_from_dstr(const darray(char) src);
void dstr_free(darray(char) darr);

size_t dstr_length(const darray(char) dstr);

darray(char) dstr_cat_cstr(darray(char) dest, const char* src);
darray(char) dstr_cat_dstr(darray(char) dest, const darray(char) src);

int dstr_cmp(const darray(char) s1, const char* s2);
int dstr_cmp_case(const darray(char) s1, const char* s2);

// Index of found stubstr on success. -1 on not found.
long dstr_find(darray(char) dstr, const char* substr);
long dstr_find_case(darray(char) dstr, const char* substr);

darray(char) dstr_replace_all(darray(char) dstr, const char* substr,
    const char* new_substr);
darray(char) dstr_replace_all_case(darray(char) dstr, const char* substr,
    const char* new_substr);

void dstr_transform_lower(darray(char) dstr);
void dstr_transform_upper(darray(char) dstr);

#endif // !_DSTRING_H_
