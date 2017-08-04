#ifndef _DARRAY_H_
#define _DARRAY_H_

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* DARRAY MEMORY LAYOUT
 * ====================
 * +--------+---------+---------+-----+------------------+
 * | header | elem[0] | elem[1] | ... | elem[capacity-1] |
 * +--------+---------+---------+-----+------------------+
 *          ^
 *          Handle to the darray points to the first
 *          element of the array.
 *
 * HEADER DATA
 * ===========
 *  size_t : sizeof contained element
 *  size_t : length of the darray
 *  size_t : capacity of the darray
 *  void*  : tmp pointer for use in array functions
 */

/**@function
 * @brief Allocate an darray of `nelem` elements each of size `size`.
 *
 * @param nelem : Initial number of elements in the darray.
 * @param size : Size of each element.
 *
 * @return Pointer to a new darray.
 */
static inline void* da_alloc(size_t nelem, size_t size);

/**@function
 * @brief Free a darray.
 *
 * @param darr : Target darray to be freed.
 */
static inline void da_free(void* darr);

/**@function
 * @brief Returns the number of elements in the darray.
 *
 * @param darr : Target darray.
 * @return Number of elements in the darray.
 */
static inline size_t da_length(void* darr);

/**@function
 * @brief Returns the maximum number of elements the darray can hold without
 *  resizing.
 *
 * @param darr : Target darray.
 * @return Total number of allocated elements in the darray.
 */
static inline size_t da_capacity(void* darr);

/**@function
 * @brief Returns the sizeof the contained elements in the darray.
 *
 * @param darr : Target darray.
 * @return sizeof elements in the darray.
 */
static inline size_t da_sizeof_elem(void* darr);

/**@function
 * @brief Change the length of the array to `nelem`. Data for elements with
 * index >= nelem may be lost.
 *
 * @param darr : Target darray. Upon function completion, darr may or may not
 *  point to its previous block on the heap, potentially breaking references to
 *  the darr.
 * @return Pointer to the new location of the darray upon successful function
 *  completion. If da_resize returns NULL, allocation failed and darr is left
 *  untouched.
 *
 * @note Affects the length attribute of the darray.
 */
static inline void* da_resize(void* darr, size_t nelem);

/**@function
 * @brief Ensure there is at least `nelem` valid elements available beyond the
 *  current length of the darray without requiring resizing.
 *
 * @param darr : Target darray. Upon function completion, darr may or may not
 *  point to its previous block on the heap, potentially breaking references to
 *  the darr.
 * @return Pointer to the new location of the darray upon successful function
 *  completion. If da_reserve returns NULL, allocation failed and darr is left
 *  untouched.
 *
 * @note Does not affect the length attribute of the darray.
 */
static inline void* da_reserve(void* darr, size_t nelem);

/**@macro
 * @brief Push a value to the back of darr.
 *
 * @param darr : const lvalue pointing to the target darray.
 * @param value : Value to be pushed onto the array.
 *
 * @note Affects the length of the darray.
 * @note This macro implimentation is the fast version of da_spush. Unlike the
 *  rest of the API, failed allocations from da resizing with da push blow up
 *  your program as reallocs are always reassigned back to the darr param. With
 *  this version of push, the user sacrifices safety for speed.
 */
#define /* void */da_push(/* void* */darr, /* ARRAY TYPE */value)              \
                                                           _da_push(darr, value)

/**@macro
 * @brief Push a value to the back of darr. This is the safe version of da_push.
 *
 * @param darr : const lvalue pointing to the target darray.
 * @param value : Value to be pushed onto the array.
 * @param backup : lvalue that will store a backup of darr in in case of
 *  resize failure.
 *
 * @note Affects the length of the darray.
 * @note If malloc fails to allocate memory during automatic array resizeing
 *  a backup of the darray will be saved to the backup param and darr will be
 *  set to NULL.
 */
#define /* void* */da_spush(/* void* */darr, /* ARRAY TYPE */value,            \
    /* void* */backup)                                                         \
                                              _da_safe_push(darr, value, backup)

/**@macro
 * @brief Remove a value from the back of darr and return it.
 *
 * @param darr : lvalue pointing to the target darray.
 *
 * @return Value popped off of the back of the darray.
 *
 * @note Affects the length of the darray.
 * @note da_pop will never reallocate memory, so popping is always
 *  allocation-safe.
 */
#define da_pop(/* void* */darr)                                                \
                                                                   _da_pop(darr)

/**@macro
 * @brief Insert a value to into darr at the specified index, pushing the rest
 *  of the values past index in darr back one.
 *
 * @param darr : const lvalue pointing to the target darray.
 * @param index : Array index where the new value will appear.
 * @param value : Value to be inserted onto the array.
 *
 * @note Affects the length of the darray.
 * @note This macro implimentation is the fast version of da_sinsert. Unlike the
 *  rest of the API, failed allocations from da resizing with da push blow up
 *  your program as reallocs are always reassigned back to the darr param. With
 *  this version of insert, the user sacrifices safety for speed.
 */
#define /* void */da_insert(/* void* */darr, /* size_t */index,                \
    /* ELEM TYPE */value)                                                      \
                                                  _da_insert(darr, index, value)

/**@macro
 * @brief Insert a value to into darr at the specified index, pushing the rest
 *  of the values past index in darr back one. This is the safe version of
 *  da_insert.
 *
 * @param darr : const lvalue pointing to the target darray.
 * @param index : Array index where the new value will appear.
 * @param value : Value to be inserted onto the array.
 * @param backup : lvalue that will store a backup of darr in in case of
 *  resize failure.
 *
 * @note Affects the length of the darray.
 * @note If malloc fails to allocate memory during automatic array resizeing
 *  a backup of the darray will be saved to the backup param and darr will be
 *  set to NULL.
 */
#define /* void* */da_sinsert(/* void* */darr, /* size_t */index,              \
    /* ELEM TYPE */value, /* void* */backup)                                   \
                                     _da_safe_insert(darr, index, value, backup)

/**@macro
 * @brief Remove the value at index from darr and return it, moving the rest if
 *  the values past index up one.
 *
 * @param darr : const lvalue pointing to the target darray.
 * @param index : Array index of the value to be removed.
 *
 * @return Value removed from the darray.
 *
 * @note Affects the length of the darray.
 * @note da_remove will never reallocate memory, so removing is always
 *  allocation-safe.
 */
#define /* ARRAY TYPE */da_remove(/* void* */darr, /* size_t */index)          \
                                                         _da_remove(darr, index)

///////////////////////////////// DEFINITIONS //////////////////////////////////
#define DA_SIZEOF_ELEM_OFFSET 0
#define DA_LENGTH_OFFSET      (1*sizeof(size_t))
#define DA_CAPACITY_OFFSET    (2*sizeof(size_t))
#define DA_HANDLE_OFFSET      (3*sizeof(size_t) + 1*sizeof(void*))

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
#define DA_NEW_CAPACITY_FROM_LENGTH(length) \
    (length < DA_CAPACITY_MIN ? DA_CAPACITY_MIN : (length*DA_CAPACITY_FACTOR))

static inline void* da_alloc(size_t nelem, size_t size)
{
    size_t capacity = DA_NEW_CAPACITY_FROM_LENGTH(nelem);
    void* mem = malloc(capacity*size + DA_HANDLE_OFFSET);
    if (mem == NULL)
    {
        return mem;
    }
    (*(size_t*)((char*)mem + DA_SIZEOF_ELEM_OFFSET)) = size;
    (*(size_t*)((char*)mem + DA_LENGTH_OFFSET))      = nelem;
    (*(size_t*)((char*)mem + DA_CAPACITY_OFFSET))    = capacity;
    return (char*)mem + DA_HANDLE_OFFSET;
}

static inline void da_free(void* darr)
{
    free(DA_HEAD_FROM_HANDLE(darr));
}

static inline size_t da_length(void* darr)
{
    return *DA_P_LENGTH_FROM_HANDLE(darr);
}

static inline size_t da_capacity(void* darr)
{
    return *DA_P_CAPACITY_FROM_HANDLE(darr);
}

static inline size_t da_sizeof_elem(void* darr)
{
    return *DA_P_SIZEOF_ELEM_FROM_HANDLE(darr);
}

static inline void* da_resize(void* darr, size_t nelem)
{
    size_t new_capacity = DA_NEW_CAPACITY_FROM_LENGTH(nelem);
    void* ptr = realloc(DA_HEAD_FROM_HANDLE(darr),
        new_capacity*da_sizeof_elem(darr) + DA_HANDLE_OFFSET);
    if (ptr == NULL)
    {
        return NULL;
    }
    darr = ptr;
    *((size_t*)((char*)ptr + DA_CAPACITY_OFFSET)) = new_capacity;
    *((size_t*)((char*)ptr + DA_LENGTH_OFFSET))   = nelem;
    return (char*)ptr + DA_HANDLE_OFFSET;
}

static inline void* da_reserve(void* darr, size_t nelem)
{
    size_t curr_capacity = da_capacity(darr);
    size_t curr_length = da_length(darr);
    size_t min_capacity = curr_length + nelem;
    if (curr_capacity >= min_capacity)
    {
        return darr;
    }
    size_t new_capacity = DA_NEW_CAPACITY_FROM_LENGTH(min_capacity);
    void* ptr = realloc(DA_HEAD_FROM_HANDLE(darr),
        new_capacity*da_sizeof_elem(darr) + DA_HANDLE_OFFSET);
    if (ptr == NULL)
    {
        return NULL;
    }
    *((size_t*)((char*)ptr + DA_CAPACITY_OFFSET)) = new_capacity;
    return (char*)ptr + DA_HANDLE_OFFSET;
}

#define /* void* */_da_push(/* void* */darr, /* ARRAY TYPE */value)            \
do                                                                             \
{                                                                              \
    register size_t* __p_len = DA_P_LENGTH_FROM_HANDLE(darr);                  \
    if (*__p_len == *DA_P_CAPACITY_FROM_HANDLE(darr))                          \
    {                                                                          \
        (darr) = da_resize((darr), *__p_len);                                  \
        __p_len  = DA_P_LENGTH_FROM_HANDLE(darr);                              \
    }                                                                          \
    (darr)[(*__p_len)++] = (value);                                            \
}while(0)

#define /* void* */_da_safe_push(/* void* */darr, /* ARRAY TYPE */value,       \
    /* void* */backup)                                                         \
do                                                                             \
{                                                                              \
    register size_t* __p_len = DA_P_LENGTH_FROM_HANDLE(darr);                  \
    if (*__p_len == *DA_P_CAPACITY_FROM_HANDLE(darr))                          \
    {                                                                          \
        (backup) = (darr);                                                     \
        (darr) = da_resize((darr), *__p_len);                                  \
        __p_len  = DA_P_LENGTH_FROM_HANDLE(darr);                              \
        if ((darr) == NULL)                                                    \
        {                                                                      \
            /* Allocation failed, but we still have the original darray */     \
            /* stored in the back backup variable. */                          \
            break;                                                             \
        }                                                                      \
        /* Allocation succeeded continue on as normal. */                      \
    }                                                                          \
    (darr)[(*__p_len)++] = (value);                                            \
}while(0)

#define /* ARRAY TYPE */_da_pop(/* void* */darr)                               \
(                                                                              \
    (darr)[--(*DA_P_LENGTH_FROM_HANDLE(darr))]                                 \
)

#define /* void */_da_insert(/* void* */darr, /* size_t */index, value)        \
do                                                                             \
{                                                                              \
    register size_t* __p_len  = DA_P_LENGTH_FROM_HANDLE(darr);                 \
    register size_t __index = (index);                                         \
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

#define _da_safe_insert(/* void* */darr, /* size_t */index,                    \
    /* ELEM TYPE*/value, /* void* */backup )                                   \
do                                                                             \
{                                                                              \
    register size_t* __p_len  = DA_P_LENGTH_FROM_HANDLE(darr);                 \
    register size_t __index = (index);                                         \
    if ((*__p_len) == (*DA_P_CAPACITY_FROM_HANDLE(darr)))                      \
    {                                                                          \
        (backup) = (darr);                                                     \
        (darr) = da_resize((darr), *__p_len);                                  \
        if ((darr) == NULL)                                                    \
        {                                                                      \
            /* Allocation failed, but we still have the original darray */     \
            /* stored in the back backup variable. */                          \
            break;                                                             \
        }                                                                      \
        /* Allocation succeeded continue on as normal. */                      \
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
// Performes the following transform:
// [0][1][2][3][rest...] => [0][2][3][1][rest...]
//     ^                           ^
//     target index                target moved to back
static inline void* _da_remove_mem_mov(
    void* darr,
    size_t target_index,
    size_t length,
    size_t elsz
)
{
    register void* tmp = malloc(elsz); // TODO: avoid memory allocation
    assert(tmp != NULL); // TODO: avoid using assert
    memcpy(tmp, (char*)darr + target_index*elsz, elsz); // tmp = arr[target]
    memmove(
        (char*)darr + target_index*elsz,
        (char*)darr + (target_index+1)*elsz,
        elsz*(length-target_index-1)
    );
    memcpy((char*)darr + (length-1)*elsz, tmp, elsz); // arr[length-1] = tmp
    free(tmp);
}
#pragma GCC diagnostic pop

#define /* ARRAY TYPE */_da_remove(/* void* */darr, /* size_t */index)         \
(                                                                              \
    (/* "then" paren(s) */                                                     \
    /* move element to be removed to the back of the array */                  \
    _da_remove_mem_mov(                                                        \
        (darr),                                                                \
        index,                                                                 \
        *DA_P_LENGTH_FROM_HANDLE(darr),                                        \
        *DA_P_SIZEOF_ELEM_FROM_HANDLE(darr))                                   \
    ), /* then */                                                              \
    /* return darr[--length] (i.e the removed element) */                      \
    (darr)[--(*DA_P_LENGTH_FROM_HANDLE(darr))]                                 \
)

#endif // !_DARRAY_H_
