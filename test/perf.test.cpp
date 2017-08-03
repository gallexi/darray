#include "../darray.h"

#include <stdio.h>
#include <time.h>
#include <vector>

#define CARR      "built in array         : "
#define DARR      "dynamic array          : "
#define DARR_S    "dynamic array (safe)   : "
#define VECTOR    "std::vector            : "
#define VECTOR_RF "std::vector (range-for): "
#define SMALL_SIZE 100
#define MED_SIZE   100000
#define LARGE_SIZE 100000000

void fill_pre_sized(void);
void fill_push_back(void);
void insert_front(void);
void insert_rand(void);
void remove_front(void);
void remove_rand(void);

int main(void)
{
    srand(time(NULL));
    fill_pre_sized(); putchar('\n');
    fill_push_back(); putchar('\n');
    insert_front();   putchar('\n');
    insert_rand();    putchar('\n');
    remove_front();   putchar('\n');
    remove_rand();
}

int clock_to_msec(clock_t c)
{
    return (((double)c) * 1000) / CLOCKS_PER_SEC;
}

void print_elapsed_time(clock_t begin, clock_t end)
{
    clock_t c = end-begin;
    printf("%u clocks | %u msec\n", c, clock_to_msec(c));
}

void print_numelem(size_t n)
{
    printf("vvv %d ELEMENTS vvv\n", n);
}

// FILL ////////////////////////////////////////////////////////////////////////
void fill_pre_sized_helper(size_t max_sz)
{
    clock_t begin;
    clock_t end;
    int* arr;
    int* darr;
    std::vector<int> vec;

    print_numelem(max_sz);

    printf(CARR);
    arr = malloc(max_sz*sizeof(int));
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        arr[i] = rand();
    }
    end = clock();
    free(arr);
    print_elapsed_time(begin, end);

    printf(DARR);
    darr = da_alloc(max_sz, sizeof(int));
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        darr[i] = rand();
    }
    end = clock();
    da_free(darr);
    print_elapsed_time(begin, end);

    printf(VECTOR);
    vec = std::vector<int>(max_sz);
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        vec[i] = rand();
    }
    end = clock();
    print_elapsed_time(begin, end);

    printf(VECTOR_RF);
    vec = std::vector<int>(max_sz);
    begin = clock();
    for (i : vec)
    {
        vec[i] = rand();
    }
    end = clock();
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
    clock_t begin;
    clock_t end;
    int* arr;
    int* darr;
    std::vector<int> vec;
    size_t curr_len;
    const int init_elem = 1;
    print_numelem(max_sz);

    printf(CARR);
    curr_len = init_elem;
    arr = malloc(init_elem*sizeof(int));
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
    print_elapsed_time(begin, end);

    printf(DARR);
    darr = da_alloc(init_elem, sizeof(int));
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        da_push(darr, rand());
    }
    end = clock();
    da_free(darr);
    print_elapsed_time(begin, end);

    printf(DARR_S);
    darr = da_alloc(init_elem, sizeof(int));
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        darr = da_pushs(darr, rand());
    }
    end = clock();
    da_free(darr);
    print_elapsed_time(begin, end);

    printf(VECTOR);
    vec = std::vector<int>(init_elem);
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        vec.push_back(rand());
    }
    end = clock();
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
    clock_t begin;
    clock_t end;
    int* darr;
    std::vector<int> vec;
    size_t index;
    const int init_elem = 1;

    print_numelem(max_sz);

    printf(DARR);
    darr = da_alloc(init_elem, sizeof(int));
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        da_insert(darr, 0, rand());
    }
    end = clock();
    da_free(darr);
    print_elapsed_time(begin, end);

    printf(VECTOR);
    vec = std::vector<int>(init_elem);
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        vec.insert(vec.begin(), rand());
    }
    end = clock();
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
    clock_t begin;
    clock_t end;
    int* darr;
    std::vector<int> vec;
    size_t index;
    const int init_elem = 1;

    print_numelem(max_sz);

    printf(DARR);
    darr = da_alloc(init_elem, sizeof(int));
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        da_insert(darr, rand() % da_length(darr), rand());
    }
    end = clock();
    da_free(darr);
    print_elapsed_time(begin, end);

    printf(VECTOR);
    vec = std::vector<int>(init_elem);
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        vec.insert(vec.begin() + (rand() % vec.size()), rand());
    }
    end = clock();
    print_elapsed_time(begin, end);
}

void insert_rand(void)
{
    puts("INSERT AT RANDOM INDEXES");
    puts("*results may vary significantly from run to run");
    insert_rand_helper(SMALL_SIZE);
    insert_rand_helper(MED_SIZE);
}

// REMOVE FRONT ////////////////////////////////////////////////////////////////
void remove_front_helper(size_t max_sz)
{
    clock_t begin;
    clock_t end;
    int* darr;
    std::vector<int> vec;
    const int init_elem = 1;
    int ans;
    int tot;

    print_numelem(max_sz);

    printf(DARR);
    darr = da_alloc(max_sz, sizeof(int));
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

    printf(VECTOR);
    vec = std::vector<int>(max_sz);
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        vec.push_back(i);
    }
    tot = 0;
    for (size_t i = 0; i < max_sz; ++i)
    {
        ans = vec[0];
        vec.erase(vec.begin());
        tot += ans;
    }
    end = clock();
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
    clock_t begin;
    clock_t end;
    int* darr;
    std::vector<int> vec;
    const int init_elem = 1;
    int ans;
    int tot;

    print_numelem(max_sz);

    printf(DARR);
    darr = da_alloc(max_sz, sizeof(int));
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

    printf(VECTOR);
    vec = std::vector<int>(max_sz);
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        vec.push_back(i);
    }
    register size_t veci;
    tot = 0;
    for (size_t i = 0; i < max_sz; ++i)
    {
        veci = rand() % vec.size();
        ans = vec[veci];
        vec.erase(vec.begin() + veci);
        tot += ans;
    }
    end = clock();
    print_elapsed_time(begin, end);
}

void remove_rand(void)
{
    puts("REMOVE AT RANDOM INDEXES");
    puts("*results may vary significantly from run to run");
    remove_rand_helper(SMALL_SIZE);
    remove_rand_helper(MED_SIZE);
}
