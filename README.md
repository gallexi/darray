# Dynamic C Arrays

**NOTE: THIS README IS UNDER CONSTRUCTION. DO NOT TAKE IT AT FACE VALUE**

## INTRO
This library provides an implementation of dynamic arrays in C with similar in feel and functionality to C++'s std::vector.

Darrays are implemented much like std::vector where a buffer of some size is allocated for a user requested N element array, expanding to fit additional elements as needed. The number of elements in use (length), the total number of elements the darray can store without requiring expansion (capacity), and the `sizeof` the contained element is stored at the front of the darray in a header section. The user is passed a handle to the darray's data section (i.e. the array itself) and it is this handle that is used by both the library and by the user for operations on the darray.
```
+--------+---------+---------+-----+------------------+
| header | elem[0] | elem[1] | ... | elem[capacity-1] |
+--------+---------+---------+-----+------------------+
         ^
         Handle to the darray points to the first
         element of the array.
```
Because a handle to the darray is a pointer directly to the array data segment, random access of elements within the array uses the same syntax as built-in arrays.
```C
foo* my_arr = da_alloc(10, sizeof(foo));
foo some_element = my_arr[4]; // works as expected
```

## USE
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
// create a darray with an initial length of 15
foo* my_arr = da_alloc(15, sizeof(foo));
````
Be aware that the `size` parameter is stored internally by the darray and is used
throughout the library for pointer math. If the `size` parameter doesn't match
the `sizeof` the base element many darray functions **will** have undefined behavior.
```C
// this will result in undefined behavior even though alignment may be valid
int* bad_idea = da_alloc(24, 1);
```
Freeing memory is as easy as calling `da_free` on a darray.
```C
da_free(my_arr);
```
Due to the fact that the handle to the darray is not actually the start of the darray's memory block, using `free` from `stdlib.h` on a darray will cause a runtime error.

#### Resizing
If you know how many elements a darray will need to hold for a particular section of code you can use `da_resize` or `da_reserve` to allocate proper storage within the darray ahead of time.
```C
// returns pointer the the newly resized darray
void* da_resize(void* darr, size_t nelem);
```
```C
// returns pointer the the new darray with reserved space
void* da_reserve(void* darr, size_t nelem);
```
Resizeing a darray will set the length of the darray to `nelem`, reallocating memory if neccesary. Downsizeing from a larger length to a smaller length will not alter values in the range `[0:nelem-1]`. Upsizing from a smaller length to a larger length will preserve the values of all previous elements, though additional elements within the darray may contain garbage values.
```C
foo* my_arr = da_alloc(15, sizeof(foo)); // initial length of 15
my_arr = da_resize(my_arr, 25); // new length of 25
```
Reserving space in a darray will reallocate memory as neccesary such that the darray can hold an additional `nelem` elements after insertion. The fundemental difference between resizing and reserving is that `da_resize` will alter both the length and capacity of the darray, while `da_reserve` will only alter the capacity of the darray.
```C
foo* my_arr = da_alloc(15, sizeof(foo)); // initial length of 15
my_arr = da_reserve(my_arr, 50);
/* length is still 15, but you can now insert/push
 * up to 50 values without reallocation
 */
```
Note that `da_alloc` and `da_reserve` will return pointers to the darray after function completion which may or may not point to the same location in memory as before function execution depending on whether reallocation was required or not. **Always** assume pointer invalidation.

Also note that if reallocation fails both `da_alloc` and `da_reserve` will return `NULL`, and the original darray will be left untouched.

#### Value Insertion
There are two main insertion functions `da_insert` and `da_push`, implemented as macros, both of which will insert a value into the darray and incriment the darray's length.
```C
#define /* void */da_insert(/* void* */darr, /* size_t */index, /* ELEM TYPE */value) \
    /* ...macro implementation */
```
```C
#define /* void */da_push(/* void* */darr, /* ELEM TYPE */value) \
    /* ...macro implementation */
```
Both macros may reassign memory behind the scenes, but unlike other functions in the library, assignment back to `darr` is automatic (for performance reasons). Again, always assume pointer invalidation (i.e. be weary of multiple references to the same darray).

What if allocation fails inside `da_insert` or `da_push`? Well unlike the rest of the library, these functions sacrifice safety for speed. If reallocation fails during insertion, a `NULL` pointer might get dereferenced and your program could blow up. That sounds pretty bad, but as it turns out in practice this almost never happens, so the library optimizes for it. In C++, pushing/inserting to a vector without catching a `std::bad_alloc` exception is more or less the same as these "unsafe" insertion macros. It is so rare for malloc to fail that it's almost never worth thinking about.

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

#### Value Removal
Removing values from a darray is a much more straightforward process, because the library will never perform reallocation when removing a value. Two functions (again implemented as macros) `da_remove` and `da_pop` are the mirrored versions of `da_insert` and `da_push` removeing/returning the target value and decrimenting the length of the darray. Neither macro will invalidate a pointer to the darray.
```C
#define /* ARRAY TYPE */da_remove(/* void* */darr, /* size_t */index) \
    /* ...macro implementation */
```
```C
#define /* ARRAY TYPE */da_pop(/* void* */darr) \
    /* ...macro implementation */
```

#### Accessing Header Data
Darrays know their own length, capacity, and `sizeof` the contained elements. All of this data lives in the darray header and can be accessed through the following functions:
```C
size_t da_length(void* darr);
```
```C
size_t da_capacity(void* darr);
```
```C
size_t da_sizeof_elem(void* darr);
```


## LIBRARY GOALS
### Halt propagation of bad boilerplate ლ(ಠ益ಠლ)
Every C programmer has at some point in their career written this snippit of code:
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
The code is lengthy, space-inefficient, prone to copy-paste errors, and requires the use two separate variables for what is at its core a single data structure (at least in the abstract sense). A lot of programmers have mixed opionions about the C++ STL, but I think we can all admit that the following code is a lot nicer on the eyes and is a lot clearer at first glance:
```C++
std::vector<int> vec;
for (int i = 0; i < N; ++i)
{
    vec.push_back(foo());
}
```
The darray library aims to eliminate the code seen first example and replace it with something more like what we saw in the second example. That same code snippit written with a darray would look like:
```C
int* darr = da_alloc(10, sizeof(int));
for (int i = 0; i < N; ++i)
{
    da_push(da, foo());
}
```
The code still looks like a C program, but gets rid of the jankiness found in the first example. Darrays give the user a set of tools that perform dynamic array operations the right way, eliminating the need for copy-pasting of potentially harmful boilerplate.

### (✿ ♥‿♥) **A E S T H E T I C S** (♥‿♥ ✿)
Clean readable code makes us happy programmers. Darrays and their associated functions/macros to provide all the functionality of a generic random access container while still looking b-e-a-utiful.

Most dynamic array implementations use something along the lines of
```C
#define arr_t(T) struct{size_t capacity, length; T* data}
```
where the container is a struct, you have to use that a weird psudo-template `#define` statement, and data access requires typing out `arr.data[i]` and `p_arr->data[i]` everywhere. This implementation certainly has some advantages over this library's implementation of dynamic arrays, but to me the struct approach seems ugly. It doesn't look or feel like the random access data structure we are all used to. Darrays let you work with the data directly, and feel much more like what we are accustomed to as C programmers.

### Speed (づ ￣ ³￣)づ
Arrays are great because they are lightning fast. Darrays are regular old arrays under the hood so all the optimization you get from built-in arrays automatically get pulled into darrays. The library ships with a set of performance tests so you can see how darrays perform in relation to built-in arrays and std::vector.

## LICENSE
MIT
