#include "../darray.h"

#include <stdio.h>
#include <time.h>
#include <vector>

#define CARR   "built in array      : "
#define DARR   "dynamic array       : "
#define DARR_S "dynamic array (safe): "
#define VECTOR "std::vector         : "
#define SMALL_SIZE 100
#define MED_SIZE   100000
#define LARGE_SIZE 100000000

void fill_pre_sized(void);
void fill_push_back(void);

int main(void)
{
    srand(time(NULL));
    fill_pre_sized();
    putchar('\n');
    fill_push_back();
}

double clock_to_msec(clock_t c)
{
    return (((double)c) * 1000) / CLOCKS_PER_SEC;
}

void fill_pre_sized_helper(size_t max_sz)
{
    clock_t begin;
    clock_t end;
    int* arr;
    int* darr;
    std::vector<int> vec;

    printf("vvv %d ELEMENTS vvv\n", max_sz);
    printf(CARR);
    arr = malloc(max_sz*sizeof(int));
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        arr[i] = rand();
    }
    end = clock();
    free(arr);
    printf("%dms\n", (int)clock_to_msec(end-begin));
    printf(DARR);
    darr = da_alloc(max_sz, sizeof(int));
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        darr[i] = rand();
    }
    end = clock();
    da_free(darr);
    printf("%dms\n", (int)clock_to_msec(end-begin));
    printf(VECTOR);
    vec = std::vector<int>(max_sz);
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        vec[i] = rand();
    }
    end = clock();
    printf("%dms\n", (int)clock_to_msec(end-begin));
}

void fill_pre_sized(void)
{
    puts("FILLING A PRE-SIZED ARRAY");
    fill_pre_sized_helper(SMALL_SIZE);
    fill_pre_sized_helper(MED_SIZE);
    fill_pre_sized_helper(LARGE_SIZE);

}

void fill_push_back_helper(size_t max_sz)
{
    clock_t begin;
    clock_t end;
    int* arr;
    int* darr;
    std::vector<int> vec;
    size_t curr_len;

    printf("vvv %d ELEMENTS vvv\n", max_sz);
    printf(CARR);
    curr_len = 1;
    arr = malloc(curr_len*sizeof(int));
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        if (i == curr_len)
        {
            curr_len = DA_NEW_CAPACITY_FROM_LENGTH(curr_len);
            arr = realloc(arr, curr_len*sizeof(int));
        }
        arr[i] = rand();
    }
    end = clock();
    free(arr);
    printf("%dms\n", (int)clock_to_msec(end-begin));

    printf(DARR);
    darr = da_alloc(max_sz, sizeof(int));
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        da_push(darr, rand());
    }
    end = clock();
    da_free(darr);
    printf("%dms\n", (int)clock_to_msec(end-begin));

    printf(DARR_S);
    darr = da_alloc(max_sz, sizeof(int));
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        darr = da_pushs(darr, rand());
    }
    end = clock();
    da_free(darr);
    printf("%dms\n", (int)clock_to_msec(end-begin));

    printf(VECTOR);
    vec = std::vector<int>(1);
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        vec.push_back(rand());
    }
    end = clock();
    printf("%dms\n", (int)clock_to_msec(end-begin));
}

void fill_push_back(void)
{
    puts("FILLING AN ARRAY VIA PUSH BACK");
    fill_push_back_helper(SMALL_SIZE);
    fill_push_back_helper(MED_SIZE);
    fill_push_back_helper(LARGE_SIZE);
}
