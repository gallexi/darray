# Dynamic C Arrays

**NOTE: THIS README IS UNDER CONSTRUCTION. DO NOT TAKE IT AT FACE VALUE**

## Intro
This library provides an implimentation of dynamic arrays in C with similar in feel and functionality to C++'s std::vector.

## Goals of this library
### Stop propagation of bad boilerplate
Every C programmer has at some point in their caree written this snippit of code:
```C
size_t curr_len = 10;
int* arr = malloc(nelems*sizeof(int));
for (int i = 0; i < some_arbitrary_number; ++i)
{
    if (i == curr_len)
    {
        curr_len = curr_len*2;
        arr = realloc(arr, curr_len*sizeof(int));
    }
    arr[i] = whatever();
}
```
The code is lengthy, space-inefficient, prone to copy-paste errors, and requires me to use two separate variables for what is at it's core a single data structure (at least in the abstract sense). A lot of programmers have mixed optionions about the C++ STL, but I think we can all admit that the following code is a lot nicer on the eyes:
```C++
std::vector<int> vec;
for (int i = 0; i < some_arbitrary_number; ++i)
{
    vec.push_back(whatever());
}
```
This library aims to eliminate the code seen first example and replace it with something like the second example. That same code chunk written with a darray would look like this:
```C
int* darr = da_alloc(10, sizeof(int));
for (int i = 0; i < some_arbitrary_number; ++i)
{
    da_push(da, whatever());
}
```
The code still looks like a C program, but gets rid of the jankey uglyness found in the first example. Darrays give the programer has a finite set of tools that perform dynamic array operations the right way, eliminating the need for copy-pasting bad boilerplate (for those who noticed the lack of NULL checking don't worry that's covered later).


### Make Sonic The Hedgehog happy
We love arrays because they are fast, and keeping that speed in a dynamic array impimentation is critical. The darray library aims to keep performance as high as possible, and only make sacrifices where it needs to. Operations such as push, pop, insert, and remove aim to be as fast or nearly as fast as if we hardcoded the operations in by hand.

### (✿ ♥‿♥) A E S T H E T I C S (✿ ♥‿♥)
The goal of programming is not to write clean code, but if we *can* make our code clean then it usually makes the jobs of us and our coworkers much easier. Darrays and their associated functions/macros aim to provide all the functionality of a generic random access container while still looking b-e-a-utiful.

Most dynamic array implimentations use something along the lines of:
```C
#define vec(T) struct{size_t capacity, length; T* data}
```
where the container is its own struct and you have to use that weird psudo-template define thing, and you have to use `vec.data[i]` or `p_vec->data[i]` everywhere to actually get to your data, and it's a while ordeal. Darrays cut just cut that whole part out and let you work with the array directly. All of the code will still look and feel like you are working with plain old arrays, but you get this additional set of features built in that are totally tubular.
Some examples:
```C
// allocation
int* my_arr = da_alloc(init_elem, sizeof(int)); // looks almost like a normal call to calloc

// adding values
da_push(my_arr, 3);
da_push(my_arr, 5);
da_insert(my_arr, 0, 7);
da_push(my_arr, 9);

// removing values
int val1 = da_remove(my_arr, 0);
int val2 = da_pop(my_arr);
da_remove(my_arr, 2);

// reserve more space in advance
da_reserve(my_arr, 100);

// resize your array
da_resize(my_arr, 25);

// length and capacity of the array
printf("this darray holds %zu elements, but can hold up to %zu elements without requiring reallocation\n", da_length(my_arr), da_capacity(my_arr));
```

### Make Sonic The Hedgehog happy
We love arrays because they are fast. Darrays are just regular old arrays under the hood so all the optimizations you get from built in arrays automatically get pulled into darrays. The library comes in with some built in performance tests so you can see how darrays perform in relation to built in arrays and std::vector.