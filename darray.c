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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
// Performes the following transform:
// [0][1][2][3][rest...] => [0][2][3][1][rest...]
//     ^                              ^
//     target index                   target moved to back
int _da_remove_mem_mov(void* darr, size_t target_index)
{
    size_t length = da_length(darr);
    size_t elsz = da_sizeof_elem(darr);

    // Try and use a temporary amount of memory for storage.
    void* tmp = malloc(elsz);
    // If the memory is avaliable then run the fast version of the algorithm
    // using memcpy and memmove.
    if (tmp != NULL)
    {
        memcpy(tmp, (char*)darr + target_index*elsz, elsz); // tmp = arr[target]
        memmove(
            (char*)darr + target_index*elsz,
            (char*)darr + (target_index+1)*elsz,
            elsz*(length-target_index-1)
        );
        memcpy((char*)darr + (length-1)*elsz, tmp, elsz); // arr[length-1] = tmp
        free(tmp);
    }
    // If the memory is NOT avaliable then run the slow, but alloc-free version
    // of the algorithm using memory swapping.
    else
    {
        char* p_curr = (char*)darr + (target_index * elsz);
        char* p_last = (char*)darr + ((length-1) * elsz);
        // Swap target and last elements.
        // [0][1][2][3][4][rest...] => [0][4][2][3][1][rest...]
        _da_memswap(p_curr, p_last, elsz);
        // Bubble the "new" target element (previously last element) to the back
        // [0][4][2][3][1][rest...] => [0][3][2][4][1][rest...]
        // [0][3][2][4][1][rest...] => [0][2][3][4][1][rest...]
        while ((p_curr += elsz) < p_last)
        {
            _da_memswap(
                p_curr,
                p_curr - elsz, // element before curr
                elsz
            );
        }
    }
}
#pragma GCC diagnostic pop

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

void* da_spush(void* darr, void* p_value)
{
    size_t len = *DA_P_LENGTH_FROM_HANDLE(darr);
    if (len == *DA_P_CAPACITY_FROM_HANDLE(darr))
    {
        void* ptr = da_reserve(darr, 1);
        if (ptr == NULL)
            return NULL;
        darr = ptr;
    }
    size_t elsz = *DA_P_SIZEOF_ELEM_FROM_HANDLE(darr);
    char* cpy_dest = (char*)darr + len*elsz;
    memcpy(cpy_dest, p_value, elsz);
    *DA_P_LENGTH_FROM_HANDLE(darr) += 1;
    return darr;
}

void* da_sinsert(void* darr, size_t index, void* p_value)
{

    if (da_length(darr) == da_capacity(darr))
    {
        void* ptr = da_reserve(darr, 1);
        if (ptr == NULL)
            return NULL;
        darr = ptr;
    }
    memmove((char*)darr+da_sizeof_elem(darr)*(index+1),
        (char*)darr+da_sizeof_elem(darr)*index,
        da_sizeof_elem(darr)*(da_length(darr)-index)
    );
    char* cpy_dest = (char*)darr + index*da_sizeof_elem(darr);
    memcpy(cpy_dest, p_value, da_sizeof_elem(darr));
    *DA_P_LENGTH_FROM_HANDLE(darr) += 1;
    return darr;
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
