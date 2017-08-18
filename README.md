# Dynamic C Arrays

## Intro
This library provides an implementation of dynamic arrays in C that is similar in functionality to C++'s std::vector.

Darrays are implemented much like std::vector. A buffer of some size is allocated for a user-requested `n` element array, and it expands to fit additional elements as needed. The number of elements in use (length), the total number of elements the darray can store without requiring expansion (capacity), and the `sizeof` the contained element is stored at the front of the darray in a header section. The user is given a handle to the darray's data section (i.e. the array itself) and it is this handle that is used by both the library and by the user for operations on the darray.
```
+--------+---------+---------+-----+------------------+
| header | elem[0] | elem[1] | ... | elem[capacity-1] |
+--------+---------+---------+-----+------------------+
         ^
         Handle to the darray points to the first
         element of the array.
```
Because a handle to the darray is a pointer directly to the array data segment, random access of elements within a darray comes with the same syntax and speed as built-in arrays.
```C
foo* my_arr = da_alloc(10, sizeof(foo));
foo some_element = my_arr[4]; // works as expected
```

## Use
### Creation and Deletion
Allocation and freeing of darrays is performed using the `da_alloc` and `da_free` functions.
```C
void* da_alloc(size_t nelem, size_t size);
```
```C
void da_free(void* darr);
```
The function signature of `da_alloc` is identical to that of calloc and is used the same way where `nelem` is the initial number of elements (length) of the array and `size` is the `sizeof` each element.
```C
// Allocate a darray on the heap with an initial length of 15.
foo* my_arr = da_alloc(15, sizeof(foo));
```
Be aware that the `size` parameter is stored internally by the darray and is used
throughout the library for pointer math. If the `size` parameter doesn't match
the `sizeof` the contained element many darray functions will have undefined behavior.
```C
// This will result in undefined behavior even though alignment may be valid.
int* bad_idea = da_alloc(24, 1);
```
Freeing memory is done by calling `da_free` on a darray.
```C
da_free(my_arr);
```
Due to the fact that the handle to a darray is not actually the start of the darray's memory block, using `free` from `stdlib.h` on a darray will cause a runtime error.

### Resizing
If you know how many elements a darray will need to hold for a particular section of code you can use `da_resize` or `da_reserve` to allocate proper storage ahead of time. The fundemental difference between resizing and reserving is that `da_resize` will alter both the length and capacity of the darray, while `da_reserve` will only alter the capacity of the darray.

```C
// Returns pointer the newly resized darray.
void* da_resize(void* darr, size_t nelem);
```
```C
// Returns pointer the new darray with reserved space.
void* da_reserve(void* darr, size_t nelem);
```
Resizing a darray will set the length of the darray to `nelem`, reallocating memory if neccesary. Downsizing from a larger length to a smaller length will not alter values in the range `[0:nelem-1]`. Upsizing from a smaller length to a larger length will preserve the values of all previous elements, though additional elements within the darray may contain garbage values.
```C
foo* my_arr = da_alloc(15, sizeof(foo)); // initial length of 15
my_arr = da_resize(my_arr, 25); // new length of 25
```
Reserving space in a darray will reallocate memory as neccesary such that the darray can hold an additional `nelem` elements after insertion. It will thus change the capacity, but not the length.
```C
foo* my_arr = da_alloc(15, sizeof(foo)); // initial length of 15
my_arr = da_reserve(my_arr, 50);
// Length is still 15, but you can now insert/push
// up to 50 values without reallocation.
```
Note that the pointers returned `da_alloc` and `da_reserve` may or may not point to the same location in memory as before function execution, depending on whether reallocation was required or not. **Always** assume pointer invalidation.

Also note that if reallocation fails both `da_alloc` and `da_reserve` will return `NULL`, and the original darray will be left untouched.

### Insertion
There are two main insertion functions `da_insert` and `da_push`, implemented as macros, both of which will insert a value into the darray and increment the darray's length.
```C
// Insert value into the darray at the specified index.
// All following elements are moved back one index.
#define /* void */da_insert(/* void* */darr, /* size_t */index, /* ELEM TYPE */value) \
    /* ...macro implementation */
```
```C
// Insert value at the back of darr (as determined by the length of darr).
#define /* void */da_push(/* void* */darr, /* ELEM TYPE */value) \
    /* ...macro implementation */
```
Both macros may reassign memory behind the scenes, but unlike other functions in the library, assignment back to `darr` is automatic (for performance reasons). Again, always assume pointer invalidation (i.e. be weary of multiple references to the same darray).

So what if allocation fails inside `da_insert` or `da_push`? Well unlike the rest of the library, these functions sacrifice safety for speed. If reallocation fails during insertion, a `NULL` pointer might get dereferenced and your program could blow up. That sounds pretty bad, but as it turns out in practice this almost never happens, so the library optimizes for it. In C++, pushing/inserting to a vector without catching a `std::bad_alloc` exception is more or less the same as these "unsafe" insertion macros. It is so rare for allocation to fail on modern systems that it's almost never worth thinking about.

But of course there are times when memory allocation can and will fail, so users **need** a way to guard against that. Both macros have separate versions that sacrifice speed for safety.
```C
#define /* void* */da_sinsert(/* void* */darr, /* size_t */index, /* ELEM TYPE */value, /* void* */backup) \
    /* ...macro implementation */
```
```C
#define /* void* */da_spush(/* void* */darr, /* ARRAY TYPE */value, /* void* */backup) \
    /* ...macro implementation */
```
These macros are identical to their unsafe counterparts save for an additional parameter `backup`. The safe macros work as such:

1. Check if memory needs to be reallocated
2. If so, store the existing array in `backup`
    + Reallocate memory
    + If reallocation fails set `darr` to `NULL` and return
    + If reallocation succeeds set `darr` to the new darray and continue
3. Insert the value and return

After each call to a safe insertion macro, you can check if `darr` is set to `NULL` and if so, restore the darray from the backup copy and handle the error.
```C
int* darr = da_alloc(init_elem, sizeof(int));
int* bak;
for (size_t i = 0; i < 1000000; ++i) // push back a million random values
{
    da_spush(darr, rand(), bak);
    if (!darr) // check if memory reallocation failed
    {
        printf("OH NO!\n");
        darr = bak; // restore backup
        do_some_error_handling();
    }
}

```

### Removal
Removing values from a darray is a much more straightforward process, because the library will never perform reallocation when removing a value. Two functions (again implemented as macros) `da_remove` and `da_pop` are the mirrored versions of `da_insert` and `da_push` removing/returning the target value and decrementing the length of the darray. Neither macro will invalidate a pointer to the darray.
```C
// Remove/return the value at the specified index from darr.
// All following elements are moved forward one index.
#define /* ARRAY TYPE */da_remove(/* void* */darr, /* size_t */index) \
    /* ...macro implementation */
```
```C
// Remove/return the value at the back of darr.
#define /* ARRAY TYPE */da_pop(/* void* */darr) \
    /* ...macro implementation */
```

### Accessing Header Data
Darrays know their own length, capacity, and `sizeof` their contained elements. All of this data lives in the darray header and can be accessed through the following functions:
```C
size_t da_length(void* darr);
```
```C
size_t da_capacity(void* darr);
```
```C
size_t da_sizeof_elem(void* darr);
```

### Additional Utilities
In addition to the functions/macros above the darray library ships with the following utilities:

----

#### da_fill
`da_fill` sets all elements in a darray to a specified value.
```C
#define /* void */da_fill(/* void* */darr, VALUE_TYPE, /* VALUE_TYPE */value) \
    /* ...macro implementation */
```
```C
// Set all elements in the range [0:da_length(darr)-1] to 15.
da_fill(darr, int, 12 + 3);
```
Due to the macro implimentation of `da_fill` the type of `value` must be specified with `VALUE_TYPE` to ensure the same value is assigned to every element of the array. Without this, a call to `da_fill` written as `da_fill(darr, rand())` would assign a different number to every element. Since `da_fill` is usually written close to the declaration of a darray, the `VALUE_TYPE` parameter is less of an inconvenience here.

----

#### da_foreach
`da_foreach` acts as a loop-block that forward iterates through all elements of a darray. In each iteration a variable with identifier `itername` will point to an element of the darray starting at its first element.
```C
#define da_foreach(/* void* */darr, ELEM_TYPE, itername) \
    /* ...macro implementation */
```
```C
// Add one to each element in darr.
da_foreach(darr, int, iter)
{
    *iter += 1;
}
```

----

#### da_foreachr
Reverse for-each loop-block. `da_foreachr` is simmilar to `da_foreach` but uses reverse iteration (from the last element to the first) rather than forward iteration through the darray.

Due to the macro implimentation of `da_foreachr` the type of elements in the darray must be specified with `ELEM_TYPE` to ensure correct iterator assignment internal to `da_foreachr`.
```C
#define da_foreachr(/* void* */darr, ELEM_TYPE, itername) \
    /* ...macro implementation */
```

## Library Goals
### Halt propagation of bad boilerplate ლ(ಠ益ಠლ)
Every C programmer has written this snippet of code at some point in their career:
```C
size_t curr_len = 10;
int* arr = malloc(curr_len*sizeof(int));
for (int i = 0; i < N; ++i)
{
    if (i == curr_len)
    {
        curr_len = curr_len*2;
        arr = realloc(arr, curr_len*sizeof(int));
    }
    arr[i] = foo();
}
```
The code is lengthy, space-inefficient, prone to copy-paste errors, and requires the use of two separate variables for what is really a single data structure (at least in the abstract sense). A lot of programmers have mixed opinions about the C++ STL, but I think we can all admit that the following code is a lot nicer on the eyes and is a lot clearer at first glance:
```C++
std::vector<int> vec;
for (int i = 0; i < N; ++i)
{
    vec.push_back(foo());
}
```
The darray library aims to eliminate the code seen first example and replace it with something more like what we saw in the second example. That same code snippet written with a darray would look like:
```C
int* darr = da_alloc(10, sizeof(int));
for (int i = 0; i < N; ++i)
{
    da_push(darr, foo());
}
```
The code still looks like a C program, but gets rid of the jankiness found in the first example. Darrays give the user a set of tools that perform dynamic array operations the right way, eliminating the need for copy-pasting of potentially harmful boilerplate.

### (✿ ♥‿♥) **A E S T H E T I C S** (♥‿♥ ✿)
Clean code makes us happy programmers. Darrays and their associated functions/macros aim to provide all the functionality of a generic random access container while still looking b-e-a-utiful.

Most dynamic array implementations use something along the lines of
```C
#define arr_t(T) struct{size_t capacity, length; T* data}
```
where the container is a struct, you have to use that weird psudo-template `#define` statement, and data access requires typing out `arr.data[i]` and `p_arr->data[i]` everywhere. This implementation certainly has some advantages, but it doesn't look or feel like the random access data structure we are used to as C programmers.

Being able to allocate a darray and use it just like a built-in array comes with **huge** benefits. We don't have to think about unfamiliar container syntax, so we can just focus on our data. With darrays, we C programmers get to keep our beautiful bracket operator syntax **and** get to use functions that let us, push, resize, get the container length, etc. like our C++ brothers get to.

### Speed (づ ￣ ³￣)づ
Arrays are great because they are lightning fast. Darrays are regular old arrays under the hood so all the optimization you get from built-in arrays is automatically pulled into darrays. The library ships with a set of performance tests so you can see how darrays perform in relation to built-in arrays and std::vector.

## LICENSE
MIT
