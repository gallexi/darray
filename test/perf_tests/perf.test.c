#include "perf.test.h"
#include "../../darray.h"

int main(void)
{
    time_t seed = time(NULL);
    printf("seed: %ld\n\n", seed);
    srand(seed);
    fill_pre_sized(); putchar('\n');
    fill_push_back(); putchar('\n');
    insert_front();   putchar('\n');
    insert_rand();    putchar('\n');
    remove_front();   putchar('\n');
    remove_rand();    putchar('\n');
    swap_rand();
}

int* arr;
int* darr;

// FILL ////////////////////////////////////////////////////////////////////////
void fill_pre_sized_helper(size_t max_sz)
{
    print_numelem(max_sz);

    printf(CARR);
    arr = (int*)malloc(max_sz*sizeof(int));
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        arr[i] = rand();
    }
    end = clock();
    free(arr);
    print_elapsed_time(begin, end);

    printf(DARR);
    darr = (int*)da_alloc(max_sz, sizeof(int));
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        darr[i] = rand();
    }
    end = clock();
    da_free(darr);
    print_elapsed_time(begin, end);

    printf(DARR_FE);
    darr = (int*)da_alloc(max_sz, sizeof(int));
    begin = clock();
    da_foreach(darr, iter)
    {
        *iter = rand();
    }
    end = clock();
    da_free(darr);
    print_elapsed_time(begin, end);
}

void fill_pre_sized(void)
{
    puts("FILLING A PRE-SIZED ARRAY");
    fill_pre_sized_helper(SMALL_SIZE);
    fill_pre_sized_helper(MED_SIZE);
    fill_pre_sized_helper(LARGE_SIZE);
}

// PUSH BACK ///////////////////////////////////////////////////////////////////
void fill_push_back_helper(size_t max_sz)
{
    print_numelem(max_sz);

    size_t curr_len;
    printf(CARR);
    curr_len = init_elem;
    arr = (int*)malloc(init_elem*sizeof(int));
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        if (i == curr_len)
        {
            curr_len = DA_NEW_CAPACITY_FROM_LENGTH(curr_len);
            arr = (int*)realloc(arr, curr_len*sizeof(int));
        }
        arr[i] = rand();
    }
    end = clock();
    free(arr);
    print_elapsed_time(begin, end);

    printf(DARR);
    darr = (int*)da_alloc(init_elem, sizeof(int));
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        da_push(darr, rand());
    }
    end = clock();
    da_free(darr);
    print_elapsed_time(begin, end);
}

void fill_push_back(void)
{
    puts("FILLING AN ARRAY VIA PUSH BACK");
    fill_push_back_helper(SMALL_SIZE);
    fill_push_back_helper(MED_SIZE);
    fill_push_back_helper(LARGE_SIZE);
}

// INSERT FRONT ////////////////////////////////////////////////////////////////
void insert_front_helper(size_t max_sz)
{
    print_numelem(max_sz);

    printf(DARR);
    darr = (int*)da_alloc(init_elem, sizeof(int));
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        da_insert(darr, 0, rand());
    }
    end = clock();
    da_free(darr);
    print_elapsed_time(begin, end);
}

void insert_front(void)
{
    puts("FILLING AN ARRAY BY INSERTING AT THE FRONT");
    insert_front_helper(SMALL_SIZE);
    insert_front_helper(MED_SIZE);
}

// INSERT RAND /////////////////////////////////////////////////////////////////
void insert_rand_helper(size_t max_sz)
{
    print_numelem(max_sz);

    printf(DARR);
    darr = (int*)da_alloc(init_elem, sizeof(int));
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        da_insert(darr, rand() % da_length(darr), rand());
    }
    end = clock();
    da_free(darr);
    print_elapsed_time(begin, end);
}

void insert_rand(void)
{
    puts("INSERT AT RANDOM INDEXES");
    puts(RESULTS_MAY_VARY);
    insert_rand_helper(MED_SIZE);
}

// REMOVE FRONT ////////////////////////////////////////////////////////////////
void remove_front_helper(size_t max_sz)
{
    int ans;
    int tot;

    print_numelem(max_sz);

    printf(DARR);
    darr = (int*)da_alloc(max_sz, sizeof(int));
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        da_push(darr, i);
    }
    tot = 0;
    for (size_t i = 0; i < max_sz; ++i)
    {
        ans = da_remove(darr, 0);
        tot += ans;
    }
    end = clock();
    da_free(darr);
    print_elapsed_time(begin, end);
}

void remove_front(void)
{
    puts("REMOVE FROM THE FRONT OF AN ARRAY");
    remove_front_helper(SMALL_SIZE);
    remove_front_helper(MED_SIZE);
}

// REMOVE RAND /////////////////////////////////////////////////////////////////
void remove_rand_helper(size_t max_sz)
{
    int ans;
    int tot;

    print_numelem(max_sz);

    printf(DARR);
    darr = (int*)da_alloc(max_sz, sizeof(int));
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        da_push(darr, i);
    }
    tot = 0;
    for (size_t i = 0; i < max_sz; ++i)
    {
        ans = da_remove(darr, rand() % da_length(darr));
        tot += ans;
    }
    end = clock();
    da_free(darr);
    print_elapsed_time(begin, end);
}

void remove_rand(void)
{
    puts("REMOVE AT RANDOM INDEXES");
    puts(RESULTS_MAY_VARY);
    remove_rand_helper(MED_SIZE);
}

// SWAP RAND ///////////////////////////////////////////////////////////////////
void swap_rand_helper(size_t array_len, size_t num_swaps)
{
    print_swaps(num_swaps);

    printf(CARR);
    arr = (int*)malloc(array_len*sizeof(int));
    begin = clock();
    int tmp;
    size_t a, b;
    for (size_t i = 0; i < num_swaps; ++i)
    {
        a = rand() % array_len;
        b = rand() % array_len;
        tmp = arr[a];
        arr[a] = arr[b];
        arr[b] = tmp;
    }
    end = clock();
    free(arr);
    print_elapsed_time(begin, end);

    printf(DARR);
    begin = clock();
    darr = (int*)da_alloc(array_len, sizeof(int));
    for (size_t i = 0; i < num_swaps; ++i)
    {
        da_swap(darr, rand() % array_len, rand() % array_len);
    }
    end = clock();
    da_free(darr);
    print_elapsed_time(begin, end);
}

void swap_rand(void)
{
    const size_t nelem = 10000;
    printf("SWAP RANDOM ELEMENTS IN A %zu LEGNTH ARRAY\n", nelem);
    swap_rand_helper(nelem, SMALL_SIZE);
    swap_rand_helper(nelem, MED_SIZE);
    swap_rand_helper(nelem, LARGE_SIZE);
}
