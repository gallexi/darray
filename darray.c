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
    struct _darray* darr = malloc(sizeof(struct _darray) + capacity*size);
    if (darr == NULL)
        return darr;
    darr->elemsz = size;
    darr->length = nelem;
    darr->capacity = capacity;
    return darr->data;
}

void* da_alloc_exact(size_t nelem, size_t size)
{
    struct _darray* darr = malloc(sizeof(struct _darray) + nelem*size);
    if (darr == NULL)
        return darr;
    darr->elemsz = size;
    darr->length = nelem;
    darr->capacity = nelem;
    return darr->data;
}

void da_free(void* darr)
{
    free(DA_P_HEAD_FROM_HANDLE(darr));
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
    size_t new_arr_size = sizeof(struct _darray) + new_capacity*da_sizeof_elem(darr);
    struct _darray* ptr = realloc(DA_P_HEAD_FROM_HANDLE(darr), new_arr_size);
    if (ptr == NULL)
        return NULL;
    ptr->length = nelem;
    ptr->capacity = new_capacity;
    return ptr->data;
}

void* da_resize_exact(void* darr, size_t nelem)
{
    size_t new_arr_size = sizeof(struct _darray) + nelem*da_sizeof_elem(darr);
    struct _darray* ptr = realloc(DA_P_HEAD_FROM_HANDLE(darr), new_arr_size);
    if (ptr == NULL)
        return NULL;
    ptr->length = nelem;
    ptr->capacity = nelem;
    return ptr->data;
}

void* da_reserve(void* darr, size_t nelem)
{
    size_t curr_capacity = da_capacity(darr);
    size_t min_capacity = da_length(darr) + nelem;
    if (curr_capacity >= min_capacity)
        return darr;
    size_t new_capacity = DA_NEW_CAPACITY_FROM_LENGTH(min_capacity);
    size_t new_arr_size = sizeof(struct _darray) + new_capacity*da_sizeof_elem(darr);
    struct _darray* ptr = realloc(DA_P_HEAD_FROM_HANDLE(darr), new_arr_size);
    if (ptr == NULL)
        return NULL;
    ptr->capacity = new_capacity;
    return ptr->data;
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
