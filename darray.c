#include "darray.h"

static inline void _da_memswap(void* p1, void* p2, size_t sz)
{
    char tmp, *a = p1, *b = p2;
    for (size_t i = 0; i < sz; ++i)
    {
        tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
}

void* da_alloc(size_t nelem, size_t size)
{
    size_t capacity = DA_NEW_CAPACITY_FROM_LENGTH(nelem);
    void* mem = malloc(capacity*size + DA_HANDLE_OFFSET);
    if (mem == NULL)
        return mem;
    (*(size_t*)((char*)mem + DA_SIZEOF_ELEM_OFFSET)) = size;
    (*(size_t*)((char*)mem + DA_LENGTH_OFFSET))      = nelem;
    (*(size_t*)((char*)mem + DA_CAPACITY_OFFSET))    = capacity;
    return (char*)mem + DA_HANDLE_OFFSET;
}

void* da_alloc_exact(size_t nelem, size_t size)
{
    void* mem = malloc(nelem*size + DA_HANDLE_OFFSET);
    if (mem == NULL)
        return mem;
    (*(size_t*)((char*)mem + DA_SIZEOF_ELEM_OFFSET)) = size;
    (*(size_t*)((char*)mem + DA_LENGTH_OFFSET))      = nelem;
    (*(size_t*)((char*)mem + DA_CAPACITY_OFFSET))    = nelem;
    return (char*)mem + DA_HANDLE_OFFSET;
}

void da_free(void* darr)
{
    free(DA_HEAD_FROM_HANDLE(darr));
}

size_t da_length(void* darr)
{
    return *DA_P_LENGTH_FROM_HANDLE(darr);
}

size_t da_capacity(void* darr)
{
    return *DA_P_CAPACITY_FROM_HANDLE(darr);
}

size_t da_sizeof_elem(void* darr)
{
    return *DA_P_SIZEOF_ELEM_FROM_HANDLE(darr);
}

void* da_resize(void* darr, size_t nelem)
{
    size_t new_capacity = DA_NEW_CAPACITY_FROM_LENGTH(nelem);
    size_t new_arr_size = new_capacity*da_sizeof_elem(darr)+DA_HANDLE_OFFSET;
    void* ptr = realloc(DA_HEAD_FROM_HANDLE(darr), new_arr_size);
    if (ptr == NULL)
        return NULL;
    *((size_t*)((char*)ptr + DA_LENGTH_OFFSET))   = nelem;
    *((size_t*)((char*)ptr + DA_CAPACITY_OFFSET)) = new_capacity;
    return (char*)ptr + DA_HANDLE_OFFSET;
}

void* da_resize_exact(void* darr, size_t nelem)
{
    size_t new_arr_size = nelem*da_sizeof_elem(darr)+DA_HANDLE_OFFSET;
    void* ptr = realloc(DA_HEAD_FROM_HANDLE(darr), new_arr_size);
    if (ptr == NULL)
        return NULL;
    *((size_t*)((char*)ptr + DA_LENGTH_OFFSET))   = nelem;
    *((size_t*)((char*)ptr + DA_CAPACITY_OFFSET)) = nelem;
    return (char*)ptr + DA_HANDLE_OFFSET;
}

void* da_reserve(void* darr, size_t nelem)
{
    size_t curr_capacity = da_capacity(darr);
    size_t min_capacity = da_length(darr) + nelem;
    if (curr_capacity >= min_capacity)
        return darr;
    size_t new_capacity = DA_NEW_CAPACITY_FROM_LENGTH(min_capacity);
    void* ptr = realloc(DA_HEAD_FROM_HANDLE(darr),
        new_capacity*da_sizeof_elem(darr) + DA_HANDLE_OFFSET);
    if (ptr == NULL)
        return NULL;
    *((size_t*)((char*)ptr + DA_CAPACITY_OFFSET)) = new_capacity;
    return (char*)ptr + DA_HANDLE_OFFSET;
}

void da_swap(void* darr, size_t index_a, size_t index_b)
{
    size_t size = da_sizeof_elem(darr);
    _da_memswap(
        ((char*)darr) + (index_a * size),
        ((char*)darr) + (index_b * size),
        size
    );
}

void* da_cat(void* dest, void* src, size_t nelem)
{
    size_t offset = da_length(dest)*da_sizeof_elem(dest);
    dest = da_resize(dest, da_length(dest)+nelem);
    if (dest == NULL)
        return NULL;
    memcpy(dest+offset, src, nelem*da_sizeof_elem(dest));
    return dest;
}
