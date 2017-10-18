#include "perf.test.h"
#include <vector>
#include <algorithm>

// FILL ////////////////////////////////////////////////////////////////////////
void fill_pre_sized_helper(size_t max_sz)
{
    std::vector<int> vec;

    vec = std::vector<int>(max_sz);
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        vec[i] = rand();
    }
    end = clock();
    print_results(VECTOR, max_sz, begin, end);

    vec = std::vector<int>(max_sz);
    begin = clock();
    for (int& e : vec)
    {
        e = rand();
    }
    end = clock();
    print_results(VECTOR_RF, max_sz, begin, end);
}

void fill_pre_sized(void)
{
    puts("FILLING A PRE-SIZED VECTOR");
    fill_pre_sized_helper(SMALL_SIZE);
    fill_pre_sized_helper(MED_SIZE);
    fill_pre_sized_helper(LARGE_SIZE);
}

// PUSH BACK ///////////////////////////////////////////////////////////////////
void fill_push_back_helper(size_t max_sz)
{
    std::vector<int> vec;

    vec = std::vector<int>(init_elem);
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        vec.push_back(rand());
    }
    end = clock();
    print_results(VECTOR, max_sz, begin, end);
}

void fill_push_back(void)
{
    puts("FILLING A VECTOR VIA PUSH BACK");
    fill_push_back_helper(SMALL_SIZE);
    fill_push_back_helper(MED_SIZE);
    fill_push_back_helper(LARGE_SIZE);
}

// INSERT FRONT ////////////////////////////////////////////////////////////////
void insert_front_helper(size_t max_sz)
{
    std::vector<int> vec;

    vec = std::vector<int>(init_elem);
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        vec.insert(vec.begin(), rand());
    }
    end = clock();
    print_results(VECTOR, max_sz, begin, end);
}

void insert_front(void)
{
    puts("FILLING A VECTOR BY INSERTING AT THE FRONT");
    insert_front_helper(SMALL_SIZE);
    insert_front_helper(MED_SIZE);
}

// INSERT RAND /////////////////////////////////////////////////////////////////
void insert_rand_helper(size_t max_sz)
{
    std::vector<int> vec;

    vec = std::vector<int>(init_elem);
    begin = clock();
    for (size_t i = 0; i < max_sz; ++i)
    {
        vec.insert(vec.begin() + (rand() % vec.size()), rand());
    }
    end = clock();
    print_results(VECTOR, max_sz, begin, end);
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
    std::vector<int> vec;
    int ans;
    int tot;

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
    print_results(VECTOR, max_sz, begin, end);
}

void remove_front(void)
{
    puts("REMOVE FROM THE FRONT OF A VECTOR");
    remove_front_helper(SMALL_SIZE);
    remove_front_helper(MED_SIZE);
}

// REMOVE RAND /////////////////////////////////////////////////////////////////
void remove_rand_helper(size_t max_sz)
{
    std::vector<int> vec;
    int ans;
    int tot;

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
    print_results(VECTOR, max_sz, begin, end);
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
    std::vector<int> vec;

    vec = std::vector<int>(array_len);
    begin = clock();
    for (size_t i = 0; i < num_swaps; ++i)
    {
        std::swap(vec[rand() % array_len], vec[rand() % array_len]);
    }
    end = clock();
    print_results(VECTOR, num_swaps, begin, end);
}

void swap_rand(void)
{

    const size_t nelem = 10000;
    printf("SWAP RANDOM ELEMENTS IN A %zu LEGNTH VECTOR\n", nelem);
    swap_rand_helper(nelem, SMALL_SIZE);
    swap_rand_helper(nelem, MED_SIZE);
    swap_rand_helper(nelem, LARGE_SIZE);
}
