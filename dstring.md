# Dstring - Darray string specialization

## Table of contents
1. [Introduction](#introduction) 
1. [API](#api)
    + [Creation and Deletion](#creation-and-deletion)
        + [dstr_alloc_empty](#dstr_alloc_empty)
        + [dstr_alloc_from_cstr](#dstr_alloc_from_cstr)
        + [dstr_alloc_from_dstr](#dstr_alloc_from_dstr)
        + [dstr_alloc_from_format](#dstr_alloc_from_format)
        + [dstr_free](#dstr_free)
    + [Dstring Data](#dstring-data)
        + [dstr_length](#dstr_length)
    + [Concatination](#concatination)
        + [dstr_cat_cstr](#dstr_cat_cstr)
        + [dstr_cat_dstr](#dstr_cat_dstr)
    + [Comparison](#comparison)
        + [dstr_cmp](#dstr_cmp)
        + [dstr_cmp_case](#dstr_cmp_case)
    + [Find and Replace Functions](#find-and-replace-functions)
        + [dstr_find](#dstr_find)
        + [dstr_find_case](#dstr_find_case)
        + [dstr_replace_all](#dstr_replace_all)
        + [dstr_replace_all_case](#dstr_replace_all_case)
    + [In-place Tranformation Functions](#in-place-tranformation-functions)
        + [dstr_transform_lower](#dstr_transform_lower)
        + [dstr_transform_upper](#dstr_transform_upper)
    + [Misc.](#misc)
        + [dstr_trim](#dstr_trim)

## Introduction
Character arrays are by far the most common array type in C. Many functions in the C standard library like `strcmp` and `printf` will work exactly the same with `darray(char)` as built-in cstrings, but some functions such as `strcpy` and `sprintf` will "break" character darrays by desynching the length property of the darray from the actual length of the string. To prevent bugs that may occur from using the C standard library functions with character darrays, the dstring extension of darrays was created. A dstring is written as `darray(char)` and refered to as such in all documentation.

By default dstrings are not included by `darray.h`. `#include <dstring.h>` to access dstring function declarations.

Generally speaking, if you are using dstrings, you should prefer the functions in this library over the C standard library when possible.

## API

### Creation and Deletion

#### dstr_alloc_empty
Allocate a dstring as the empty string `""`.

Returns a pointer to a new dstring on success. `NULL` on allocation failure.
```C
darray(char) dstr_alloc_empty(void);
```

#### dstr_alloc_from_cstr
Allocate a dstring as copy of cstring `src`. `src` may also be a dstring.

Returns a pointer to a new dstring on success. `NULL` on allocation failure.
```C
darray(char) dstr_alloc_from_cstr(const char* src);
```

#### dstr_alloc_from_dstr
Allocate a dstring as a copy of dstring `src`. Faster than `dstr_alloc_from_cstr` when copying a dstring.

Returns a pointer to a new dstring on success. `NULL` on allocation failure.
```C
darray(char) dstr_alloc_from_dstr(const darray(char) src);
```

#### dstr_alloc_from_format
Allocate a dstring using `sprintf` style formatting.

Returns a pointer to a new dstring on success. `NULL` on allocation failure.
```C
darray(char) dstr_alloc_from_format(const char* format, ...);
```

#### dstr_free
Free a dstring. Equivalent to calling `da_free` on `dstr`.
```C
void dstr_free(darray(char) dstr);
```

----

### Dstring Data

#### dstr_length
Returns the length of `dstr` without its null terminator. Equivalent to `da_length(dstr)-1`. O(1) compared to the O(n) `strlen`.
```C
size_t dstr_length(const darray(char) dstr);
```

----

### Concatination

#### dstr_cat_cstr
Append `src` to dstring `dest`. `src` may also be a dstring.

Returns a pointer to the new location of the dstring upon successful function completion. If `dstr_cat_cstr` returns `NULL`, reallocation failed and `dstr` is left untouched.
```C
darray(char) dstr_cat_cstr(darray(char) dest, const char* src);
```
Like `da_cat` references to `dest` may be invalidated across the function call. Use the return value of `dstr_cat_cstr` as truth for the location of `dest` after function completion.

#### dstr_cat_dstr
Append `src` to dstring `dest`. Faster than `dstr_cat_cstr` for cat-ing a dstring to another dstring.

Returns a pointer to the new location of the dstring upon successful function completion. If `dstr_cat_dstr` returns `NULL`, reallocation failed and `dstr` is left untouched.
```C
darray(char) dstr_cat_dstr(darray(char) dest, const darray(char) src);
```
Like `da_cat` references to `dest` may be invalidated across the function call. Use the return value of `dstr_cat_dstr` as truth for the location of `dest` after function completion.

----

### Comparison

#### dstr_cmp
Comparison function. Currently functionally equivalent to `strcmp`.
```C
int dstr_cmp(const darray(char) s1, const char* s2);
```

#### dstr_cmp_case
Comparison function. Currently functionally equivalent to `strcasecmp`.
```C
int dstr_cmp_case(const darray(char) s1, const char* s2);
```

----

### Find and Replace Functions

#### dstr_find
Returns the index of the first occurrence of `substr` in `dstr` or `-1` if `substr` was not found. Similar to Python's `str.find`.
```C
long dstr_find(darray(char) dstr, const char* substr);
```

#### dstr_find_case
Returns the index of the first case insensitive occurrence of `substr` in `dstr` or `-1` if `substr` was not found. Similar to Python's `str.find`.
```C
long dstr_find_case(darray(char) dstr, const char* substr);
```

#### dstr_replace_all
Replace all occurrences of `substr` in `dstr` with `new_str`.

Returns the new location of `dstr` after function completion. If `dstr_replace_all` returns `NULL` reallocation failed and `dstr` is left untouched.
```C
darray(char) dstr_replace_all(darray(char) dstr, const char* substr, const char* new_str);
```

#### dstr_replace_all_case
Replace all occurrences of `substr` (case insensitive) in `dstr` with `new_str`.

Returns the new location of `dstr` after function completion. If `dstr_replace_all_case` returns `NULL` reallocation failed and `dstr` is left untouched.
```C
darray(char) dstr_replace_all(darray(char) dstr, const char* substr, const char* new_str);
```

----

### In-place Tranformation Functions

#### dstr_transform_lower
Transform `dstr` to lower case in place.
```C
void dstr_transform_lower(darray(char) dstr);
```

#### dstr_transform_upper
Transform `dstr` to upper case in place.
```C
void dstr_transform_upper(darray(char) dstr);
```

----

### Misc.

#### dstr_trim
Trims leading and trailing whitespace from `dstr`.

Returns the new location of `dstr` after function completion. If `dstr_trim` returns `NULL` reallocation failed and `dstr` is left untouched.
```C
darray(char) dstr_trim(darray(char) dstr);
```
