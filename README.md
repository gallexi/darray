# Dynamic C Arrays

**NOTE: THIS README IS UNDER CONSTRUCTION. DO NOT TAKE IT AT FACE VALUE**

## Intro
This library provides an implimentation of dynamic arrays in C with similar in feel and functionality to C++'s std::vector.

## Goals of this library
### Stop propagation of bad boilerplate
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
The code is lengthy, space-inefficient, prone to copy-paste errors, and requires the use two separate variables for what is at its core a single data structure (at least in the abstract sense). A lot of programmers have mixed optionions about the C++ STL, but I think we can all admit that the following code is a lot nicer on the eyes:
```C++
std::vector<int> vec;
for (int i = 0; i < N; ++i)
{
    vec.push_back(foo());
}
```
This library aims to eliminate the code seen first example and replace it with something like the second example. That same code chunk written with a darray would look like this:
```C
int* darr = da_alloc(10, sizeof(int));
for (int i = 0; i < N; ++i)
{
    da_push(da, foo());
}
```
The code still looks like a C program, but gets rid of the jankey uglyness found in the first example. Darrays give the programer a set of tools that perform dynamic array operations the right way, eliminating the need for copy-pasting bad boilerplate (for those who noticed the lack of NULL checking don't worry that's covered later).

### (✿ ♥‿♥) A E S T H E T I C S (♥‿♥ ✿)
If we *can* make our code clean then documentation and algorithm optimization come much easier. Darrays and their associated functions/macros aim to provide all the functionality of a generic random access container while still looking b-e-a-utiful.

Most dynamic array implimentations use something along the lines of:
```C
#define ARR(T) struct{size_t capacity, length; T* data}
```
where the container is its own struct, you have to use that a weird psudo-template `#define` statement, and data access requires typeing `arr.data[i]` or `p_arr->data[i]` everywhere. This implimentation certainly has *some* advantages over this implimentation of darrays, but to me it feels, for lack of a better phrase, like a shitty std::vector instead of the random access data structure we are all used to. Darrays let you work with the underlying data directly and feel much more like array structure we are used to.

Some examples of darray **A E S T H E T I C S**:
```C
// The darray alloc function has a signature identical to calloc, taking in
// a number of elements and their size, and returning a chunk of memory as a
// void* to be used as an array just like the normal C allocation functions.
int* my_arr = da_alloc(init_elem, sizeof(int));

// reserve and resize work similar to their std::vector counterparts
my_arr = da_resize(my_arr, 50); // resize my_arr to length 50
my_arr = da_reserve(my_arr, 100); // make sure my_arr can hold an additional
                                  // 100 values without requiring reallocation

// insertion
da_push(my_arr, foo); // insert foo at the the back of my_arr
da_insert(my_arr, 3, bar); // insert bar at index 3 of my_arr, moving all
                           // following elements back one index

// deletion
int A = da_pop(my_arr); // remove/return value at the back of my_arr
int B = da_remove(my_arr, 2) // remove/return value at index 2 of my_arr,
                             // moving all following elements up one index

// darrays know their own length and capacity
printf("this darray holds %zu elements, \
    but can hold up to %zu elements without requiring reallocation\n", \
    da_length(my_arr), da_capacity(my_arr));

// random access
int C = da[0]; // yay we still get to use the [] operator!
float D = (float)da[10] / da[3];
```

### Speed ლ(ಠ益ಠლ)
Arrays are great because they are lightning fast. Darrays are regular old arrays under the hood so all the optimization you get from built in arrays automatically get pulled into darrays. The library ships with a set of performance tests so you can see how darrays perform in relation to built in arrays and std::vector.

## How it works
TODO

## API
TODO

## License
TODO