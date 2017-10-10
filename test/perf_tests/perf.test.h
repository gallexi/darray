#pragma once

#include <stdio.h>
#include <time.h>

#define CARR             "built in array         : "
#define DARR             "darray                 : "
#define DARR_FE          "darray (foreach)       : "
#define VECTOR           "std::vector            : "
#define VECTOR_RF        "std::vector (range-for): "
#define RESULTS_MAY_VARY "*results may vary significantly from run to run"
#define SMALL_SIZE 100
#define MED_SIZE   100000
#define LARGE_SIZE 100000000

long clock_to_msec(clock_t c)
{
    return (((double)c) * 1000) / CLOCKS_PER_SEC;
}

void print_elapsed_time(clock_t begin, clock_t end)
{
    clock_t c = end-begin;
    printf("%ld clocks | %ld msec\n", c, clock_to_msec(c));
}

void print_numelem(size_t n)
{
    printf("vvv %zu ELEMENTS vvv\n", n);
}

void print_swaps(size_t n)
{
    printf("vvv %zu SWAPS vvv\n", n);
}

void fill_pre_sized(void);
void fill_push_back(void);
void insert_front(void);
void insert_rand(void);
void remove_front(void);
void remove_rand(void);
void swap_rand(void);

clock_t begin;
clock_t end;
const int init_elem = 1;
