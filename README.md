# Darray - Dynamic C Arrays

## Table of contents
1. [Introduction](#introduction) 
1. [Building](#building)
1. [API](#api)
    + [Creation and Deletion](#creation-and-deletion)
        + [da_alloc](#da_alloc)
        + [da_alloc_exact](#da_alloc_exact)
        + [da_free](#da_free)
    + [Resizing](#resizing)
        + [da_resize](#da_resize)
        + [da_resize_exact](#da_resize_exact)
        + [da_reserve](#da_reserve)
    + [Insertion](#insertion)
        + [da_insert [GNU C only]](#da_insert)
        + [da_push [GNU C only]](#da_push)
    + [Removal](#removal)
        + [da_remove [GNU C only]](#da_remove)
        + [da_pop [GNU C only]](#da_pop)
    + [Accessing Header Data](#accessing-header-data)
        + [da_length](#da_length)
        + [da_capacity](#da_capacity)
        + [da_sizeof_elem](#da_sizeof_elem)
    + [Misc. Utilities](#misc-utilities)
        + [da_swap](#da_swap)
        + [da_cat](#da_cat)
        + [da_fill [GNU C only]](#da_fill)
        + [da_foreach [GNU C only]](#da_foreach)
        + [container-style type](#container-style-type)
1. [License](#license)

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
+ `make install` - Install the darray header and lib files locally (will likely require elevated permissions).
+ `make unit_tests` - Build unit tests for the darray library. The environment variable `EMU_ROOT` must be set to the root directory of [EMU](https://github.com/VictorSCushman/EMU) (the testing framework used for the darray library) for this target to build.
+ `make perf_tests` - Build performance tests comparing the darray library against both built-in arrays and `std::vector` all at `-O3` optimization.

## API

Note: The type `ELEM_TYPE` used throughout the API documentation referes towhatever the `typeof` the contained element is for a particular darray.

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

#### da_insert
Insert a value into `darr` at the specified index, moving the values beyond `index` back one element. Assignment back to the provided `darr` lvalue parameter is automatic.

Returns `true` on success, `false` on failure. If `da_insert` returns `false`, reallocation failed and `darr` is left untouched.
```C
#define /* bool */da_insert(/* ELEM_TYPE* */darr, /* size_t */index, /* ELEM_TYPE */value) \
    /* ...macro implementation */
```
Upon function completion, `darr` may or may not point to its previous block on the heap. Assignment back to the provided `darr` lvalue is automatic, but other references to darr may be invalidated.

#### da_push
Insert a value at the back of `darr`. Assignment back to the provided `darr` lvalue parameter is automatic.

Returns `true` on success, `false` on failure. If `da_insert` returns `false`, reallocation failed and `darr` is left untouched.
```C
#define /* bool */da_push(/* ELEM_TYPE* */darr, /* ELEM_TYPE */value) \
    /* ...macro implementation */
```
Upon function completion, `darr` may or may not point to its previous block on the heap. Assignment back to the provided `darr` lvalue is automatic, but other references to darr may be invalidated.

----

### Removal
Two functions `da_remove` and `da_pop` are the mirrored versions of `da_insert` and `da_push` removing/returning the target value and decrementing the length of the darray. Neither macro will invalidate a pointer to the provided darray.

#### da_remove
Remove the value at `index` from `darr` and return it, moving the values beyond `index` forward one element.

Returns the value removed from the darray.
```C
#define /* ELEM_TYPE */da_remove(/* ELEM_TYPE* */darr, /* size_t */index) \
    /*
```

#### da_pop
Remove a value from the back of `darr` and return it.

Returns the value removed from the darray.
```C
#define /* ELEM_TYPE */da_pop(/* ELEM_TYPE* */darr) \
    /* ...macro implementation */
```

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
my_darr = da_cat(my_darr, built_in_array, built_in_array_len);
```

#### da_fill
Set every element of `darr` to `value`.
```C
#define /* void */da_fill(/* ELEM_TYPE* */darr, /* ELEM_TYPE */value) \
    /* ...macro implementation */
```
```C
// Set all elements in the range [0:da_length(darr)-1] to 15.
da_fill(darr, 12+3);
```

#### da_foreach
Acts as a loop-block that forward iterates through all elements of a darray. In each iteration a variable with identifier `itername` will point to an element of the darray starting at its first element.
```C
#define da_foreach(/* ELEM_TYPE* */darr, itername) \
    /* ...macro implementation */
```
```C
int* darr = da_alloc(num_elems, sizeof(int));
// do some stuff...
// ...
// then...
// add one to each element in darr
da_foreach(darr, iter) // reads "for each *iter in darr"
{
    *iter += 1;
}
```

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

## License
MIT (contributers welcome)
