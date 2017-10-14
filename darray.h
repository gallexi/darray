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
#include <stdbool.h>
#include <string.h>

/* DARRAY MEMORY LAYOUT
 * ====================
 * +--------+---------+---------+-----+------------------+
 * | header | data[0] | data[1] | ... | data[capacity-1] |
 * +--------+---------+---------+-----+------------------+
 *          ^
 *          Handle to the darray points to the first
 *          element of the array.
 */

 /**@macro
 * @brief Type of a darray that contains elements of `type`. This should be
 *  used for function parameters/return values that explicitly require a darray,
 *  or for segments of code where it must be stated that a darray is being used.
 *
 * @param type : Type of the contained element.
 */
#define darray(type) type*

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
 * @note Affects the length of the darray.
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
 * @note Affects the length of the darray.
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
 * @note Does NOT affect the length of the darray.
 */
void* da_reserve(void* darr, size_t nelem);

/**@macro
 * @brief Insert a value at the back of `darr`. Assignment back to the provided 
 *  `darr` lvalue parameter is automatic.
 *
 * @param darr : Target darray. Upon function completion, `darr` may or may not
 *  point to its previous block on the heap. Assignment back to the provided
 *  `darr` lvalue is automatic, but other references to `darr` may be
 *  invalidated.
 * @param value : Value to be pushed onto the back of the darray.
 *
 * @return `true` on success, `false` on failure. If `da_push` returns `false`,
 *  reallocation failed and `darr` is left untouched.
 *
 * @note Affects the length of the darray.
 */
#define /* bool */da_push(/* ELEM_TYPE* */darr, /* ELEM_TYPE */value)          \
                                                           _da_push(darr, value)

/**@macro
 * @brief Remove a value from the back of `darr` and return it.
 *
 * @param darr : Target darray.
 *
 * @return Value popped off of the back of the darray.
 *
 * @note Affects the length of the darray.
 * @note `da_pop` will never reallocate memory, so popping is always
 *  allocation-safe.
 */
#define /* ELEM_TYPE */da_pop(/* ELEM_TYPE* */darr)                            \
                                                                   _da_pop(darr)

/**@macro
 * @brief Insert a value into `darr` at the specified index, moving the values
 *  beyond `index` back one element. Assignment back to the provided `darr`
 *  lvalue parameter is automatic.
 *
 * @param darr : Target darray. Upon function completion, `darr` may or may not
 *  point to its previous block on the heap. Assignment back to the provided
 *  `darr` lvalue is automatic, but other references to `darr` may be
 *  invalidated.
 * @param index : Array index where the new value will appear.
 * @param value : Value to be inserted onto the darray.
 *
 * @return `true` on success, `false` on failure. If `da_insert` returns
 * `false`, reallocation failed and `darr` is left untouched.
 *
 * @note Affects the length of the darray.
 */
#define /* bool */da_insert(/* ELEM_TYPE* */darr, /* size_t */index,           \
    /* ELEM_TYPE */value)                                                      \
                                                  _da_insert(darr, index, value)

/**@macro
 * @brief Insert `nelem` values from `src` into `darr` at the specified index,
 *  moving the values beyond `index` back `nelem` elements. Assignment back to
 *  the provided `darr` lvalue parameter is automatic.
 *
 * @param darr : Target darray. Upon function completion, `darr` may or may not
 *  point to its previous block on the heap. Assignment back to the provided
 *  `darr` lvalue is automatic, but other references to `darr` may be
 *  invalidated.
 * @param index : Array index where the new values will appear.
 * @param src : Array of elements to insert.
 * @param nelem : Number of elements from `src` to insert.
 *
 * @return `true` on success, `false` on failure. If `da_insert` returns
 * `false`, reallocation failed and `darr` is left untouched.
 *
 * @note Affects the length of the darray.
 */
#define /* bool */da_insert_arr(/* ELEM_TYPE* */darr, /* size_t */index,       \
    /* ELEM_TYPE* */src, /* size_t */nelem)                                    \
                                         _da_insert_arr(darr, index, src, nelem)

/**@macro
 * @brief Remove the value at `index` from `darr` and return it, moving the
 *  values beyond `index` forward one element.
 *
 * @param darr : Target darray.
 * @param index : Array index of the value to be removed.
 *
 * @return Value removed from the darray.
 *
 * @note Affects the length of the darray.
 * @note `da_remove` will never reallocate memory, so removing is always
 *  allocation-safe.
 */
#define /* ELEM_TYPE */da_remove(/* ELEM_TYPE* */darr, /* size_t */index)      \
                                                         _da_remove(darr, index)

/**@macro
 * @brief Remove `nelem` values starting at `index` from `darr`, moving the
 *  values beyond `index` forward `nelem` elements.
 *
 * @param darr : Target darray.
 * @param index : Array index of the start of elements to remove.
 * @param nelem : Number of elements to remove.
 *
 * @note Affects the length of the darray.
 * @note `da_remove_arr` will never reallocate memory, so removing is always
 *  allocation-safe.
 */
#define /* void */da_remove_arr(/* ELEM_TYPE* */darr, /* size_t */index,       \
    /* size_t */nelem)                                                         \
                                              _da_remove_arr(darr, index, nelem)
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
 * @brief Append `nelem` array elements from `src` to the back of darray `dest`
 *  reallocating memory in `dest` if neccesary. `src` is preserved across the
 *  call. `src` may be a built-in array or a darray.
 *
 * @param dest : Darray that will be appended to. Upon function completion,
 *  `dest` may or may not point to its previous block on the heap, potentially
 *  breaking references.
 * @param src : Start of elements to append to dest.
 * @param nelem : Number of elements from `src` to append to `dest`.
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
 * @param darr : Target darray.
 * @param value : Value to fill the array with.
 */
#define /* void */da_fill(/* ELEM_TYPE* */darr, /* ELEM_TYPE */value)          \
                                                           _da_fill(darr, value)

/**@macro
 * @brief `da_foreach` acts as a loop-block that forward iterates through all
 *  elements of `darr`. In each iteration a variable with identifier `itername`
 *  will point to an element of `darr` starting at at its first element.
 *
 * @param darr : Target darray.
 * @param itername : Identifier for the iterator within the foreach block.
 */
#define da_foreach(/* ELEM_TYPE* */darr, itername)                             \
                                                     _da_foreach(darr, itername)

/////////////////////////////////// INTERNAL ///////////////////////////////////
struct _darray
{
    size_t _elemsz, _length, _capacity;
    alignas(alignof(max_align_t)) char _data[];
};

#define DA_CAPACITY_FACTOR 1.3
#define DA_CAPACITY_MIN 10
#define DA_NEW_CAPACITY_FROM_LENGTH(length) ((length) < DA_CAPACITY_MIN ? \
    DA_CAPACITY_MIN : ((length)*DA_CAPACITY_FACTOR))

#define DA_P_HEAD_FROM_HANDLE(darr_h) (((char*)darr_h)-sizeof(struct _darray))
#define DA_P_SIZEOF_ELEM_FROM_HANDLE(darr_h) ((size_t*) \
    (DA_P_HEAD_FROM_HANDLE(darr_h) + offsetof(struct _darray, _elemsz)))
#define DA_P_LENGTH_FROM_HANDLE(darr_h) ((size_t*) \
    (DA_P_HEAD_FROM_HANDLE(darr_h) + offsetof(struct _darray, _length)))
#define DA_P_CAPACITY_FROM_HANDLE(darr_h) ((size_t*) \
    (DA_P_HEAD_FROM_HANDLE(darr_h) + offsetof(struct _darray, _capacity)))

// The following macros use GNU C and are only avaliable for compatible vendors.
#if defined(__GNUC__) || defined(__clang__) // GNU C compilers

#define /* bool */_da_push(/* ELEM_TYPE* */darr, /* ELEM_TYPE */value)         \
({                                                                             \
    bool _rtn_val = true;                                                      \
    __auto_type _p_darr = &darr;                                               \
    __auto_type _value = value;                                                \
    if (*DA_P_LENGTH_FROM_HANDLE(*_p_darr) ==                                  \
        *DA_P_CAPACITY_FROM_HANDLE(*_p_darr))                                  \
    {                                                                          \
        void* _tmp = (__typeof__(*_p_darr))da_reserve(*_p_darr, 1);            \
        if (_tmp)                                                              \
        {                                                                      \
            *_p_darr = (__typeof__(*_p_darr))_tmp;                             \
            (*_p_darr)[(*DA_P_LENGTH_FROM_HANDLE(*_p_darr))++] = _value;       \
        }                                                                      \
        else /* allocation failure */                                          \
        {                                                                      \
            _rtn_val = false;                                                  \
        }                                                                      \
    }                                                                          \
    else                                                                       \
    {                                                                          \
        (*_p_darr)[(*DA_P_LENGTH_FROM_HANDLE(*_p_darr))++] = _value;           \
    }                                                                          \
    /* return */_rtn_val;                                                      \
})

#define /* ELEM_TYPE */_da_pop(/* ELEM_TYPE* */darr)                           \
({                                                                             \
    __auto_type _darr = darr;                                                  \
    /* return */(_darr)[--(*DA_P_LENGTH_FROM_HANDLE(_darr))];                  \
})

#define _da_move_and_insert(p_darr, index, value)                              \
memmove(                                                                       \
    (*p_darr)+index+1,                                                         \
    (*p_darr)+index,                                                           \
    (*DA_P_SIZEOF_ELEM_FROM_HANDLE(*p_darr)) *                                 \
        ((*DA_P_LENGTH_FROM_HANDLE(*p_darr))-index)                            \
);                                                                             \
(*p_darr)[index] = value;                                                      \
(*DA_P_LENGTH_FROM_HANDLE(*p_darr))++;                                         \

#define _da_move_and_insert_arr(p_darr, index, src, nelem)                     \
memmove(                                                                       \
    (*p_darr)+index+nelem,                                                     \
    (*p_darr)+index,                                                           \
    (*DA_P_SIZEOF_ELEM_FROM_HANDLE(*p_darr)) *                                 \
        ((*DA_P_LENGTH_FROM_HANDLE(*p_darr))-index)                            \
);                                                                             \
memcpy(                                                                        \
    (*p_darr)+index,                                                           \
    src,                                                                       \
    (*DA_P_SIZEOF_ELEM_FROM_HANDLE(*p_darr)) * (nelem)                         \
);                                                                             \
(*DA_P_LENGTH_FROM_HANDLE(*p_darr)) += nelem;                                  \

#define /* bool */_da_insert(/* ELEM_TYPE* */darr, /* size_t */index,          \
    /* ELEM_TYPE */value)                                                      \
({                                                                             \
    bool _rtn_val = true;                                                      \
    __auto_type _p_darr = &darr;                                               \
    size_t _index = index;                                                     \
    __auto_type _value = value;                                                \
    if (*DA_P_LENGTH_FROM_HANDLE(*_p_darr) ==                                  \
        *DA_P_CAPACITY_FROM_HANDLE(*_p_darr))                                  \
    {                                                                          \
        void* _tmp = (__typeof__(*_p_darr))da_reserve(*_p_darr, 1);            \
        if (_tmp)                                                              \
        {                                                                      \
            *_p_darr = (__typeof__(*_p_darr))_tmp;                             \
            _da_move_and_insert(_p_darr, _index, _value);                      \
        }                                                                      \
        else /* allocation failure */                                          \
        {                                                                      \
            _rtn_val = false;                                                  \
        }                                                                      \
    }                                                                          \
    else                                                                       \
    {                                                                          \
        _da_move_and_insert(_p_darr, _index, _value);                          \
    }                                                                          \
    /* return */_rtn_val;                                                      \
})

#define /* bool */_da_insert_arr(/* ELEM_TYPE* */darr, /* size_t */index,      \
    /* ELEM_TYPE* */src, /* size_t */nelem)                                    \
({                                                                             \
    bool _rtn_val = true;                                                      \
    __auto_type _p_darr = &darr;                                               \
    size_t _index = index;                                                     \
    __auto_type _src = src;                                                    \
    size_t _nelem = nelem;                                                     \
    if (*DA_P_LENGTH_FROM_HANDLE(*_p_darr)+nelem >=                            \
        *DA_P_CAPACITY_FROM_HANDLE(*_p_darr))                                  \
    {                                                                          \
        void* _tmp = (__typeof__(*_p_darr))da_reserve(*_p_darr, 1);            \
        if (_tmp)                                                              \
        {                                                                      \
            *_p_darr = (__typeof__(*_p_darr))_tmp;                             \
            _da_move_and_insert_arr(_p_darr, _index, _src, _nelem);            \
        }                                                                      \
        else /* allocation failure */                                          \
        {                                                                      \
            _rtn_val = false;                                                  \
        }                                                                      \
    }                                                                          \
    else                                                                       \
    {                                                                          \
        _da_move_and_insert_arr(_p_darr, _index, _src, _nelem);                \
    }                                                                          \
    /* return */_rtn_val;                                                      \
})

#define /* ELEM_TYPE */_da_remove(/* ELEM_TYPE* */darr, /* size_t */index)     \
({                                                                             \
    __auto_type _darr = darr;                                                  \
    size_t _index = index;                                                     \
    __auto_type _rtn_val = _darr[_index];                                      \
    memmove(                                                                   \
        _darr+_index,                                                          \
        _darr+_index+1,                                                        \
        (*DA_P_SIZEOF_ELEM_FROM_HANDLE(_darr)) *                               \
            ((*DA_P_LENGTH_FROM_HANDLE(_darr))-_index-1)                       \
    );                                                                         \
    (*DA_P_LENGTH_FROM_HANDLE(_darr))--;                                       \
    /* return */_rtn_val;                                                      \
})

#define /* void */_da_remove_arr(/* ELEM_TYPE* */darr, /* size_t */index,      \
    /* size_t */nelem)                                                         \
do                                                                             \
{                                                                              \
    __auto_type _darr = darr;                                                  \
    size_t _index = index;                                                     \
    size_t _nelem = nelem;                                                     \
    memmove(                                                                   \
        _darr+_index,                                                          \
        _darr+_index+_nelem,                                                   \
        (*DA_P_SIZEOF_ELEM_FROM_HANDLE(_darr)) *                               \
            ((*DA_P_LENGTH_FROM_HANDLE(_darr))-_index-_nelem)                  \
    );                                                                         \
    (*DA_P_LENGTH_FROM_HANDLE(_darr)) -= _nelem;                               \
}while(0)

#define /* void */_da_fill(/* ELEM_TYPE* */darr, /* ELEM_TYPE */value)         \
do                                                                             \
{                                                                              \
    __auto_type _darr = darr;                                                  \
    __auto_type _value = value;                                                \
    size_t _len = *DA_P_LENGTH_FROM_HANDLE(_darr);                             \
    for (size_t _indx = 0; _indx < _len; ++_indx)                              \
    {                                                                          \
        _darr[_indx] = _value;                                                 \
    }                                                                          \
}while(0)

#define DA_MERGE_IDENTIFIER_HELPER(a, b) a##b
#define DA_MERGE_IDENTIFIER(a, b) DA_MERGE_IDENTIFIER_HELPER(a, b)

#define _da_foreach(/* ELEM_TYPE* */darr, itername)                            \
bool __attribute__ ((unused))                                                  \
    DA_MERGE_IDENTIFIER(_da_first_iteration, __LINE__) = true;                 \
void* __attribute__ ((unused))                                                 \
    DA_MERGE_IDENTIFIER(_da_stop, __LINE__);                                   \
for (__auto_type itername = darr;                                              \
    ({                                                                         \
        if (DA_MERGE_IDENTIFIER(_da_first_iteration, __LINE__))                \
        {                                                                      \
            DA_MERGE_IDENTIFIER(_da_stop, __LINE__) =                          \
                itername + da_length(itername);                                \
            DA_MERGE_IDENTIFIER(_da_first_iteration, __LINE__) = false;        \
        }                                                                      \
    }), (char*)itername < (char*)DA_MERGE_IDENTIFIER(_da_stop, __LINE__);      \
    ++itername)

#endif // !GNU C compilers
#endif // !_DARRAY_H_
