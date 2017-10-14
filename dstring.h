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
#include <ctype.h>

darray(char) dstr_alloc_empty();
darray(char) dstr_alloc_from_cstr(char* src);
darray(char) dstr_alloc_from_dstr(darray(char) src);
void dstr_free(darray(char) darr);

darray(char) dstr_cat_cstr(darray(char) dest, char* src);
darray(char) dstr_cat_dstr(darray(char) dest, darray(char) src);

int dstr_cmp(const char* s1, const char* s2);
int dstr_cmp_case(const char* s1, const char* s2);

size_t dstr_length(const darray(char) dstr);

void dstr_transform_lower(darray(char) dstr);
void dstr_transform_upper(darray(char) dstr);

#endif // !_DSTRING_H_
