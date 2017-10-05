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
#ifndef _DARRAY_H_
#define _DARRAY_H_

#include <stdalign.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif // !__cplusplus

/* DARRAY MEMORY LAYOUT
 * ====================
 * +--------+--------+- ------+-----+-----------------+
 * | header | arr[0] | arr[1] | ... | arr[capacity-1] |
 * +--------+--------+--------+-----+-----------------+
 *          ^
 *          Handle to the darray points to the first
 *          element of the array.
 *
 * HEADER DATA
 * ===========
 *  + size_t : sizeof contained element
 *  + size_t : length of the darray
 *  + size_t : capacity of the darray
 *  + any extra padding to align the header with max_align_t
 */

/**@function
 * @brief Allocate a darray of `nelem` elements each of size `size`.
 *
 * @param nelem : Initial number of elements in the darray.
 * @param size : `sizeof` each element.
 *
 * @return Pointer to a new darray.
 */
void* da_alloc(size_t nelem, size_t size);

/**@function
 * @brief Allocate a darray of `nelem` elements each of size `size`. The
 *  capacity of the darray will be be exactly `nelem`.
 *
 * @param nelem : Initial number of elements in the darray.
 * @param size : `sizeof` each element.
 *
 * @return Pointer to a new darray.
 */
void* da_alloc_exact(size_t nelem, size_t size);

/**@function
 * @brief Free a darray.
 *
 * @param darr : Target darray to be freed.
 */
void da_free(void* darr);

/**@function
 * @brief Returns the number of elements in a darray.
 *
 * @param darr : Target darray.
 *
 * @return Number of elements in the `darr`.
 */
size_t da_length(void* darr);

/**@function
 * @brief Returns the maximum number of elements a darray can hold without
 *  requiring memory reallocation.
 *
 * @param darr : Target darray.
 *
 * @return Total number of allocated elements in `darr`.
 */
size_t da_capacity(void* darr);

/**@function
 * @brief Returns the `sizeof` contained elements in a darray.
 *
 * @param darr : Target darray.
 *
 * @return `sizeof` elements in `darr`.
 */
size_t da_sizeof_elem(void* darr);

/**@function
 * @brief Change the length of a darray to `nelem`. Data in elements with
 *  indices >= `nelem` may be lost when downsizing.
 *
 * @param darr : Target darray. Upon function completion, `darr` may or may not
 *  point to its previous block on the heap, potentially breaking references.
 * @param nelem : New length of the darray.
 *
 * @return Pointer to the new location of the darray upon successful function
 *  completion. If `da_resize` returns `NULL`, reallocation failed and `darr` is
 *  left untouched.
 *
 * @note Affects the length attribute of the darray.
 */
void* da_resize(void* darr, size_t nelem);

/**@function
 * @brief Change the length of a darray to `nelem`. The new capacity of the
 *  darray will be be exactly `nelem`. Data in elements with indices >=
 *  `nelem` may be lost when downsizing.
 *
 * @param darr : Target darray. Upon function completion, `darr` may or may not
 *  point to its previous block on the heap, potentially breaking references.
 * @param nelem : New length/capacity of the darray.
 *
 * @return Pointer to the new location of the darray upon successful function
 *  completion. If `da_resize_exact` returns `NULL`, reallocation failed and
 *  `darr` is left untouched.
 *
 * @note Affects the length attribute of the darray.
 */
void* da_resize_exact(void* darr, size_t nelem);

/**@function
 * @brief Guarantee that at least `nelem` elements beyond the current length of
 *  a darray can be inserted/pushed without requiring memory reallocation.
 *
 * @param darr : Target darray. Upon function completion, `darr` may or may not
 *  point to its previous block on the heap, potentially breaking references.
 * @param nelem : Number of additional elements that may be inserted.
 *
 * @return Pointer to the new location of the darray upon successful function
 *  completion. If `da_reserve` returns `NULL`, reallocation failed and `darr`
 *  is left untouched.
 *
 * @note Does NOT affect the length attribute of the darray.
 */
void* da_reserve(void* darr, size_t nelem);

/**@macro
 * @brief Insert a value at the back of `darr`.
 *
 * @param darr : constexpr lvalue darray.
 * @param value : Value to be pushed onto the back of the darray.
 *
 * @note Affects the length of the darray.
 * @note This macro implimentation is the fast version of `da_spush`. Unlike the
 *  rest of the API, failed allocations from resizing with `da_push` may
 *  cause your program to blow up as reallocs are always reassigned back to
 *  `darr`. With this version of push, the user sacrifices safety for speed.
 * @note `darr` may be evaluated multiple times.
 */
#define /* void */da_push(/* void* */darr, /* ELEM_TYPE */value)               \
                                                           _da_push(darr, value)

/**@function
 * @brief Push a value to the back of `darr`. This is the safe version of
 *  `da_push`.
 *
 * @param darr : Target darray. Upon function completion, `darr` may or may not
 *  point to its previous block on the heap, potentially breaking references.
 * @param p_value : Pointer to the value to be pushed onto the back of the
 *  darray.
 *
 * @return Pointer to the new location of the darray upon successful function
 *  completion. If `da_spush` returns `NULL`, reallocation failed and `darr` is
 *  left untouched.
 * @note Affects the length of the darray.
 */
void* da_spush(void* darr, void* p_value);

/**@macro
 * @brief Remove a value from the back of `darr` and return it.
 *
 * @param darr : constexpr lvalue darray.
 *
 * @return Value popped off of the back of the darray.
 *
 * @note Affects the length of the darray.
 * @note `da_pop` will never reallocate memory, so popping is always
 *  allocation-safe.
 * @note `darr` may be evaluated multiple times.
 */
#define /* ELEM_TYPE */da_pop(/* void* */darr)                                 \
                                                                   _da_pop(darr)

/**@macro
 * @brief Insert a value into `darr` at the specified index, moving the values
 * beyond `index` back one element.
 *
 * @param darr : constexpr lvalue darray.
 * @param index : Array index where the new value will appear.
 * @param value : Value to be inserted onto the darray.
 *
 * @note Affects the length of the darray.
 * @note This macro implimentation is the fast version of `da_sinsert`. Unlike
 *  the rest of the API, failed allocations from resizing with `da_insert` may
 *  cause your program to blow up as reallocs are always reassigned back to
 *  `darr`. With this version of insert, the user sacrifices safety for
 *  speed.
 * @note `darr` may be evaluated multiple times.
 */
#define /* void */da_insert(/* void* */darr, /* size_t */index,                \
    /* ELEM_TYPE */value)                                                      \
                                                  _da_insert(darr, index, value)

 /**@function
 * @brief Insert a value into `darr` at the specified index, moving the values
 * beyond `index` back one element. This is the safe version of `da_insert`.
 *
 * @param darr : Target darray. Upon function completion, `darr` may or may not
 *  point to its previous block on the heap, potentially breaking references.
 * @param p_value : Pointer to the value to be inserted into the darray.
 *
 * @return Pointer to the new location of the darray upon successful function
 *  completion. If `da_sinsert` returns `NULL`, reallocation failed and `darr`
 *  is left untouched.
 * @note Affects the length of the darray.
 */
void* da_sinsert(void* darr, size_t index, void* p_value);

/**@macro
 * @brief Remove the value at `index` from `darr` and return it, moving the
 * values beyond `index` forward one element.
 *
 * @param darr : constexpr lvalue darray.
 * @param index : Array index of the value to be removed.
 *
 * @return Value removed from the darray.
 *
 * @note Affects the length of the darray.
 * @note `da_remove` will never reallocate memory, so removing is always
 *  allocation-safe.
 * @note `darr` may be evaluated multiple times.
 */
#define /* ELEM_TYPE */da_remove(/* void* */darr, /* size_t */index)           \
                                                         _da_remove(darr, index)

/**@function
 * @brief Swap the values of the two specified elements of `darr`.
 *
 * @param darr : Target darray.
 * @param index_a : Index of the first element.
 * @param index_b : Index of the second element.
 *
 * @note da_swap uses byte by byte memory swapping to exchange two elements,
 *  which in many cases is slower than using an intermediate temp variable for
 *  the swap. The classic
 *      tmp = darr[index_a];
 *      darr[index_a] = darr[index_b];
 *      darr[index_b] = tmp;
 *  may be faster than da_swap in many cases as optimizations including full
 *  word assignment and large scale memcopy generated by the compiler almost
 *  always outperform a byte by byte swap.
 */
void da_swap(void* darr, size_t index_a, size_t index_b);

/**@macro
 *
 * @brief Append `nelem` array elements from `src` to the back of darray `dest`
 *  reallocating memory in `dest` if neccesary. `src` is preserved across the
 *  call. `src` may be a built-in array or a darray.
 *
 * @param dest : Darray that will be appended to. Upon function completion,
 *  `dest` may or may not point to its previous block on the heap, potentially
 *  breaking references.
 * @param src : Start of elements to append to dest.
 * @param nelem : Number of elements from src to append to dest.
 *
 * @return Pointer to the new location of the darray upon successful function
 *  completion. If `da_cat` returns `NULL`, reallocation failed and `darr`
 *  is left untouched.
 *
 * @note Unlike `strcat` in libc, references to `dest` may be broken across
 *  a function call to `da_cat`. The return value of `da_cat` should be used as
 *  truth for the location of `dest` after function completion.
 */
void* da_cat(void* dest, void* src, size_t nelem);

/**@macro
 * @brief Set every element of `darr` to `value`.
 *
 * @param darr : constexpr lvalue darray.
 * @param VALUE_TYPE : Type of `value`.
 * @param value : Value to fill the array with.
 *
 * @note `darr` may be evaluated multiple times.
 */
#define /* void */da_fill(/* void* */darr, VALUE_TYPE, /* VALUE_TYPE */value)  \
                                               _da_fill(darr, VALUE_TYPE, value)

/**@macro
 * @brief `da_foreach` acts as a loop-block that forward iterates through all
 *  elements of `darr`. In each iteration a variable with identifier `itername`
 *  will point to an element of `darr` starting at at its first element.
 *
 * @param darr : constexpr lvalue darray.
 * @param ELEM_TYPE : Type of the elements of darr.
 * @param itername : Identifier for the iterator within the foreach block.
 */
#define da_foreach(/* void* */darr, ELEM_TYPE, itername)                       \
                                          _da_foreach(darr, ELEM_TYPE, itername)

/**@macro
 * @brief `da_foreachr` acts as a loop-block that reverse iterates through all
 *  elements of `darr`. In each iteration a variable with identifier `itername`
 *  will point to an element of `darr` starting at its last element.
 *
 * @param darr : constexpr lvalue darray.
 * @param ELEM_TYPE : Type of the elements of darr.
 * @param itername : Identifier for the iterator within the foreachr block.
 */
#define da_foreachr(/* void* */darr, ELEM_TYPE, itername)                      \
                                         _da_foreachr(darr, ELEM_TYPE, itername)

/**@macro
 * @brief Type of a darray that contains elements of `type`. This should be
 *  used for function parameters/return values that explicitly require a darray,
 *  or for segments of code where it must be stated that a darray is being used.
 *
 * @param type : Type of the contained element.
 */
#define darray(type) type*

// UPPER CASE versions of macro "functions"
#define DA_PUSH     da_push
#define DA_POP      da_pop
#define DA_INSERT   da_insert
#define DA_REMOVE   da_remove
#define DA_FILL     da_fill
#define DA_FOREACH  da_foreach
#define DA_FOREACHR da_foreachr

/////////////////////////////////// INTERNAL ///////////////////////////////////
static const size_t DA_SIZEOF_ELEM_OFFSET  = 0;
static const size_t DA_LENGTH_OFFSET   = 1*sizeof(size_t);
static const size_t DA_CAPACITY_OFFSET = 2*sizeof(size_t);
static const size_t DA_HANDLE_OFFSET = \
    (4*sizeof(size_t)) % alignof(max_align_t) == 0 ? \
    4*sizeof(size_t) : 3*alignof(max_align_t);

#define DA_HEAD_FROM_HANDLE(darr_h) \
    (((char*)(darr_h)) - DA_HANDLE_OFFSET)
#define DA_P_SIZEOF_ELEM_FROM_HANDLE(darr_h) \
    ((size_t*)(DA_HEAD_FROM_HANDLE(darr_h) + DA_SIZEOF_ELEM_OFFSET))
#define DA_P_LENGTH_FROM_HANDLE(darr_h) \
    ((size_t*)(DA_HEAD_FROM_HANDLE(darr_h) + DA_LENGTH_OFFSET))
#define DA_P_CAPACITY_FROM_HANDLE(darr_h) \
    ((size_t*)(DA_HEAD_FROM_HANDLE(darr_h) + DA_CAPACITY_OFFSET))

#define DA_CAPACITY_FACTOR 1.3
#define DA_CAPACITY_MIN 10
#define DA_NEW_CAPACITY_FROM_LENGTH(length) ((length) < DA_CAPACITY_MIN ? \
    DA_CAPACITY_MIN : ((length)*DA_CAPACITY_FACTOR))

int _da_remove_mem_mov(void* darr, size_t target_index);

#define /* void */_da_push(/* void* */darr, /* ELEM_TYPE */value)              \
do                                                                             \
{                                                                              \
    size_t* __p_len = DA_P_LENGTH_FROM_HANDLE(darr);                           \
    if (*__p_len == *DA_P_CAPACITY_FROM_HANDLE(darr))                          \
    {                                                                          \
        (darr) = da_resize((darr), *__p_len);                                  \
        __p_len  = DA_P_LENGTH_FROM_HANDLE(darr);                              \
    }                                                                          \
    (darr)[(*__p_len)++] = (value);                                            \
}while(0)

#define /* ELEM_TYPE */_da_pop(/* void* */darr)                                \
(                                                                              \
    (darr)[--(*DA_P_LENGTH_FROM_HANDLE(darr))]                                 \
)

#define /* void */_da_insert(/* void* */darr, /* size_t */index,               \
    /* ELEM_TYPE */value)                                                      \
do                                                                             \
{                                                                              \
    size_t* __p_len  = DA_P_LENGTH_FROM_HANDLE(darr);                          \
    size_t __index = (index);                                                  \
    if ((*__p_len) == (*DA_P_CAPACITY_FROM_HANDLE(darr)))                      \
    {                                                                          \
        (darr) = da_resize((darr), *__p_len);                                  \
        __p_len = DA_P_LENGTH_FROM_HANDLE(darr);                               \
    }                                                                          \
    memmove(                                                                   \
        (darr)+(__index)+1,                                                    \
        (darr)+(__index),                                                      \
        (*DA_P_SIZEOF_ELEM_FROM_HANDLE(darr))*((*__p_len)-(__index))           \
    );                                                                         \
    (darr)[__index] = (value);                                                 \
    (*__p_len)++;                                                              \
}while(0)

#define /* ELEM_TYPE */_da_remove(/* void* */darr, /* size_t */index)          \
(                                                                              \
    (/* "then" paren(s) */                                                     \
    /* move element to be removed to the back of the array */                  \
    _da_remove_mem_mov(darr, index)                                            \
    ), /* then */                                                              \
    /* return darr[--length] (i.e the removed element) */                      \
    (darr)[--(*DA_P_LENGTH_FROM_HANDLE(darr))]                                 \
)

#define /* void */_da_fill(/* void* */darr, VALUE_TYPE, /* VALUE_TYPE */value) \
do                                                                             \
{                                                                              \
    size_t __len = *DA_P_LENGTH_FROM_HANDLE(darr);                             \
    VALUE_TYPE __value = (value);                                              \
    for (size_t __i = 0; __i < __len; ++__i)                                   \
    {                                                                          \
        (darr)[__i] = (__value);                                               \
    }                                                                          \
}while(0)

#define _da_foreach(/* void* */darr, ELEM_TYPE, itername)                      \
for (ELEM_TYPE* itername = darr;                                               \
    itername < (darr) + da_length(darr);                                       \
    itername++)

#define _da_foreachr(/* void* */darr, ELEM_TYPE, itername)                     \
for (ELEM_TYPE* itername = &(darr)[da_length(darr)-1];                         \
    itername >= (darr);                                                        \
    itername--)

#ifdef __cplusplus
} // !extern "C"
#endif // !__cplusplus
#endif // !_DARRAY_H_
