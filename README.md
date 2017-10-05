# Darray - Dynamic C Arrays

## Table of contents
1. [Introduction](#introduction) 
2. [Building](#building)
3. [API](#api)
    + [Creation and Deletion](#creation-and-deletion)
        + [da_alloc](#da_alloc)
        + [da_alloc_exact](#da_alloc_exact)
        + [da_free](#da_free)
    + [Resizing](#resizing)
        + [da_resize](#da_resize)
        + [da_resize_exact](#da_resize_exact)
        + [da_reserve](#da_reserve)
    + [Insertion](#insertion)
        + [da_insert](#da_insert)
        + [da_push](#da_push)
        + [da_sinsert](#da_sinsert)
        + [da_spush](#da_spush)
    + [Removal](#removal)
        + [da_remove](#da_remove)
        + [da_pop](#da_pop)
    + [Accessing Header Data](#accessing-header-data)
        + [da_length](#da_length)
        + [da_capacity](#da_capacity)
        + [da_sizeof_elem](#da_sizeof_elem)
    + [Misc. Utilities](#misc-utilities)
        + [da_swap](#da_swap)
        + [da_cat](#da_cat)
        + [da_fill](#da_fill)
        + [da_foreach](#da_foreach)
        + [da_foreachr](#da_foreachr)
        + [container-style type](#container-style-type)
4. [*A Note About Macros and Constant Expressions](#constexpr-note_)
5. [License](#license)

## Introduction
This library provides an implementation of dynamic arrays in C that is similar in functionality to C++'s `std::vector`.

Darrays are implemented much like `std::vector`. A buffer of some size is allocated for a user-requested `n` element array and expands to fit additional elements as needed. The number of elements in use (length), the total number of elements the darray can store without requiring resizing (capacity), and the `sizeof` the contained element is stored at the front of the darray in a header section. The user is given a handle to the darray's data section (i.e. the array itself) and it is this handle that is used by both the library and by the user for operations on the darray.
```
+--------+--------+- ------+-----+-----------------+
| header | arr[0] | arr[1] | ... | arr[capacity-1] |
+--------+--------+--------+-----+-----------------+
         ^
         Handle to the darray points to the first
         element of the array.
```
Because a handle to the darray is a pointer directly to the array data segment, random access of elements within a darray comes with the same syntax and speed as built-in arrays.
```C
foo* my_arr = da_alloc(10, sizeof(foo));
foo some_element = my_arr[4]; // works as expected
```

## Building
The `makefile` included with the darray library contains four targets. All targets (except install) output to the `build` directory.

+ `make build` - Build the darray static library.
+ `make install` - Install the darray header and lib files locally.
+ `make unit_tests` - Build unit tests for the darray library. The environment variable `EMU_ROOT` must be set to the root directory of [EMU](https://github.com/VictorSCushman/EMU) (the testing framework used for the darray library) for this target to build.
+ `make perf_tests` - Build performance tests comparing the darray library against both built-in arrays and `std::vector` all at `-O3` optimization.

## API

### Creation and Deletion

#### da_alloc
Allocate a darray of `nelem` elements each of size `size`.

Returns a pointer to a new darray.
```C
void* da_alloc(size_t nelem, size_t size);
```
The function signature of `da_alloc` is identical to that of `calloc` and is used the same way where `nelem` is the initial number of elements (length) of the array and `size` is the `sizeof` each element. Elements of a darray initially contain garbage values.

```C
// Allocate a darray on the heap with an initial length of 15.
foo* my_arr = da_alloc(15, sizeof(foo));
```

Be aware that the `size` parameter is stored internally by the darray and is used throughout the library for pointer math. If the `size` parameter doesn't match the `sizeof` the contained element many darray functions will have undefined behavior.

Unlike `calloc`, `da_alloc` can be called with `nelem` equal to `0`. You will simply be left with a darray of zero elements. The function call
```C
foo* my_stack = da_alloc(0, sizeof(foo));
```
can be used to declare an empty array-based stack of `foo`.

#### da_alloc_exact
Allocate a darray of `nelem` elements each of size `size`. The capacity of the darray will be be exactly `nelem`.

Returns a pointer to a new darray.
```C
void* da_alloc_exact(size_t nelem, size_t size);
```
This version of `da_alloc` is useful for fixed-size arrays and/or environments with tight memory constraints.

#### da_free
Free a darray.
```C
void da_free(void* darr);
```
Due to the fact that the handle to a darray is not actually the start of the darray's memory block, using `free` from `stdlib.h` on a darray will cause a runtime error.

----

### Resizing
If you know how many elements a darray will need to hold for a particular section of code you can use `da_resize`, `da_resize_exact`,or `da_reserve` to allocate proper storage ahead of time. The fundamental difference between resizing and reserving is that `da_resize` and `da_resize_exact` will alter both the length and capacity of the darray, while `da_reserve` will only alter the capacity of the darray.

Pointers returned by `da_resize`, `da_resize_exact`, and `da_reserve` may or may not point to the same location in memory as before function execution, depending on whether memory reallocation was required or not. **Always** assume pointer invalidation.

#### da_resize
Change the length of a darray to `nelem`. Data in elements with indices >= `nelem` may be lost when downsizing.

Returns a pointer to the new location of the darray upon successful function completion. If `da_resize` returns `NULL`, allocation failed and `darr` is left untouched.
```C
void* da_resize(void* darr, size_t nelem);
```
```C
foo* my_arr = da_alloc(15, sizeof(foo)); // initial length of 15
my_arr = da_resize(my_arr, 25); // new length of 25
```

#### da_resize_exact
Change the length of a darray to `nelem`. The new capacity of the darray will be be exactly `nelem`. Data in elements with indices >= `nelem` may be lost when downsizing.

Pointer to the new location of the darray upon successful function completion. If `da_resize_exact` returns `NULL`, reallocation failed and `darr` is left untouched.

```C
void* da_resize_exact(void* darr, size_t nelem);
```

This version of `da_resize` is useful for fixed-size arrays and/or environments with tight memory constraints.

#### da_reserve
Guarantee that at least `nelem` elements beyond the current length of a darray can be inserted/pushed without requiring memory reallocation.

Returns a pointer to the new location of the darray upon successful function completion. If `da_reserve` returns `NULL`, allocation failed and `darr` is left untouched.
```C
void* da_reserve(void* darr, size_t nelem);
```
```C
foo* my_arr = da_alloc(15, sizeof(foo)); // initial length of 15
my_arr = da_reserve(my_arr, 50);
// length is still 15, but you can now insert/push
// up to 50 values without reallocation
```

----

### Insertion
There are two main insertion functions `da_insert` and `da_push`, implemented as macros, both of which will insert a value into a darray and increment the darray's length. Both macros may reassign memory behind the scenes, but unlike other functions in the library, assignment back to `darr` is automatic (for performance/convenience reasons). Again, always assume pointer invalidation (i.e. be weary of multiple references to the same darray).

Unlike the rest of the library if reallocation fails during insertion with `da_insert` or `da_push` a `NULL` pointer might get dereferenced and your program could blow up. That sounds pretty bad, but as it turns out in practice this almost never happens, so the library optimizes for it. In C++, pushing/inserting into a `std::vector` without catching a `std::bad_alloc` exception is more or less the same as these "unsafe" insertion macros. It is so rare for allocation to fail on modern systems that it's almost never worth thinking about.

Of course there are times when memory allocation can and will fail, and users **need** a way to guard against it. Both macros have separate implimentations as functions that sacrifice speed/convenience for memory/double-macro-evaluation safety.

#### da_insert
Insert a value into `darr` at the specified index, moving the values beyond `index` back one element.
```C
#define /* void */da_insert(/* void* */darr, /* size_t */index, /* ELEM_TYPE */value) \
    /* ...macro implementation */
```

Note: see [a note about macros and contant expressions](#constexpr-note_) below about required constant expression for `darr` in `da_insert`.

#### da_push
Insert a value at the back of `darr`.
```C
#define /* void */da_push(/* void* */darr, /* ELEM_TYPE */value) \
    /* ...macro implementation */
```

Note: see [a note about macros and contant expressions](#constexpr-note_) below about required constant expression for `darr` in `da_push`.

#### da_sinsert

Insert a value into `darr` at the specified index, moving the values beyond `index` back one element. This is the safe version of `da_insert`.

Returns a pointer to the new location of the darray upon successful function completion. If `da_sinsert` returns `NULL`, reallocation failed and `darr` is left untouched.
```C
void* da_sinsert(void* darr, size_t index, void* p_value);
```

#### da_spush
Push a value to the back of `darr`. This is the safe version of `da_push`.

Returns a pointer to the new location of the darray upon successful function completion. If `da_spush` returns `NULL`, reallocation failed and `darr` is left untouched.
```C
void* da_spush(void* darr, void* p_value);
```

----

### Removal
Removing values from a darray is a much more straightforward process, because the library will never perform reallocation when removing a value. Two functions (again implemented as macros) `da_remove` and `da_pop` are the mirrored versions of `da_insert` and `da_push` removing/returning the target value and decrementing the length of the darray. Neither macro will invalidate a pointer to the provided darray.

#### da_remove
Remove the value at `index` from `darr` and return it, moving the values beyond `index` forward one element.

Returns the value removed from the darray.
```C
#define /* ELEM_TYPE */da_remove(/* void* */darr, /* size_t */index) \
    /*
```

Note: see [a note about macros and contant expressions](#constexpr-note_) below about required constant expression for `darr` in `da_remove`.

#### da_pop
Remove a value from the back of `darr` and return it.

Returns the value removed from the darray.
```C
#define /* ELEM_TYPE */da_pop(/* void* */darr) \
    /* ...macro implementation */
```

Note: see [a note about macros and contant expressions](#constexpr-note_) below about required constant expression for `darr` in `da_pop`.

----

### Accessing Header Data
Darrays know their own length, capacity, and `sizeof` their contained elements. All of this data lives in the darray header and can be accessed through the following functions:

#### da_length
Returns the number of elements in `darr`.
```C
size_t da_length(void* darr);
```

#### da_capacity
Returns the maximum number of elements `darr` can hold without requiring memory reallocation.
```C
size_t da_capacity(void* darr);
```

#### da_sizeof_elem
Returns the `sizeof` contained elements in a `darr`.
```C
size_t da_sizeof_elem(void* darr);
```

----

### Misc. Utilities
In addition to the functions/macros above the darray library ships with the following utilities:

#### da_swap
Swap the values of the two specified elements of `darr`.
```C
void da_swap(void* darr, size_t index_a, size_t index_b);
```

#### da_cat
Append `nelem` array elements from `src` to the back of darray `dest` reallocating memory in `dest` if neccesary. `src` is preserved across the call. `src` may be a built-in array or a darray.

Returns a pointer to the new location of the darray upon successful function completion. If `da_cat` returns `NULL`, reallocation failed and `darr` is left untouched.
```C
void* da_cat(void* dest, void* src, size_t nelem)
```
Unlike `strcat` and `strncat` in libc, references to `dest` may be broken across a function call to `da_cat`. The return value of `da_cat` should be used as truth for the location of `dest` after function completion.
```C
// cat darray src to the back of darray dest
dest = da_cat(dest, src, da_length(src));
```

An easy way to create a darray from a built-in array is to cat the built-in array onto a zero-length darray.
```C
foo* my_darr = da_alloc(0, sizeof(foo));
my_darr = da_cat(my_darr, foo_array, length_of_foo_array);
```

#### da_fill
Set every element of `darr` to `value`.
```C
#define /* void */da_fill(/* void* */darr, VALUE_TYPE, /* VALUE_TYPE */value) \
    /* ...macro implementation */
```
```C
// Set all elements in the range [0:da_length(darr)-1] to (int)(12+3).
da_fill(darr, int, 12+3);
```
Due to the macro implementation of `da_fill` the type of `value` must be specified with `VALUE_TYPE` to ensure the same value is assigned to every element of the array. Without this, a call to `da_fill` written as `da_fill(darr, rand())` would assign a different number to every element.

Note: see [a note about macros and contant expressions](#constexpr-note_) below about required constant expression for `darr` in `da_fill`.

#### da_foreach
Acts as a loop-block that forward iterates through all elements of a darray. In each iteration a variable with identifier `itername` will point to an element of the darray starting at its first element.
```C
#define da_foreach(/* void* */darr, ELEM_TYPE, itername) \
    /* ...macro implementation */
```
Due to the macro implementation of `da_foreach` the type of elements in the darray must be specified with `ELEM_TYPE` to ensure correct iterator assignment internal to `da_foreach`.
```C
int* darr = da_alloc(num_elems, sizeof(int));
// do some stuff...
// ...
// then...
// add one to each element in darr
da_foreach(darr, int, iter) // reads "for each int *iter in darr"
{
    *iter += 1;
}
```

Note: see [a note about macros and contant expressions](#constexpr-note_) below about required constant expression for `darr` in `da_foreach`.

#### da_foreachr
Reverse for-each loop-block. `da_foreachr` is similar to `da_foreach` but uses reverse iteration (from the last element to the first) rather than forward iteration through the darray.
```C
#define da_foreachr(/* void* */darr, ELEM_TYPE, itername) \
    /* ...macro implementation */
```
Due to the macro implementation of `da_foreachr` the type of elements in the darray must be specified with `ELEM_TYPE` to ensure correct iterator assignment internal to `da_foreachr`.

Note: see [a note about macros and contant expressions](#constexpr-note_) below about required constant expression for `darr` in `da_foreachr`.

#### container-style type
The container-style type provides a way to explicitly state that an array is a darray.
```C
#define darray(type) type*
```
Notice that `darray(foo)` is really just syntactic sugar for `foo*` just like how `array-of-bar` can be written in C as `bar*`. Since darrays are just normal built-in arrays under the hood, this `#define` should come as no surprise.

This method of typing is especially useful in function declarations and sparsely commented code where you may want to inform readers that the memory being handled by a code segment uses darray operations.
```C
// darray(foo) can be used instead of foo* to let a user know that darray
// operations will be used on arr somewhere in my_func, so if a normal foo*
// is passed in the program will crash
void some_func(int i, darray(foo) arr, char* str);
```

----

<a name="constexpr-note_"> </a>

### *A Note About Macros and Constant Expressions
C lacks true generics, so the following "functions" in the darray library implemented as macros to allow psudo-container-generics. As a result these macros suffer from [double evaluation](https://dustri.org/b/min-and-max-macro-considered-harmful.html) of their `darr` parameter:

+ da_push
+ da_pop
+ da_insert
+ da_remove
+ da_fill
+ da_foreach
+ da_foreachr

This double evaluation is unavoidable without either requiring the user to specify a darray's element type in every macro or using the non-portable [typeof](https://gcc.gnu.org/onlinedocs/gcc/Typeof.html) specifier. For convenience to the user and portability across compilers neither of these options have been chosen for the darray library. Instead the user is **required** to use a [constant expression](http://northstar-www.dartmouth.edu/doc/ibmcxx/en_US/doc/language/concepts/cuexpcon.htm) (not necessarily a const variable) for the `darr` parameter in darray macro functions knowing that it may be double evaluated. This is the overwhelmingly common case and is not a problem most of the time, but the user should be aware of it for the case where some clever bit of code may potentially cause a bug.

For those who prefer explicit reminders that they are using macros with double evaluation ALL CAPITAL versons of each macro are defined. For example `da_push` and `DA_PUSH` will expand to the same macro.

## License
MIT (contributers welcomed)
