#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CARR             "built-in array"
#define DARR             "darray"
#define DARR_FE          "darray (foreach)"
#define VECTOR           "std::vector"
#define VECTOR_RF        "std::vector (range-for)"
#define RESULTS_MAY_VARY "*results may vary significantly from run to run"
#define HR40             "========================================"
#define SMALL_SIZE 100
#define MED_SIZE   100000
#define LARGE_SIZE 100000000

#ifdef __cplusplus
#   define MAX_WIDTH_TYPE_STR VECTOR_RF
#else
#   define MAX_WIDTH_TYPE_STR DARR_FE
#endif // !__cplusplus
#define WIDTH_OF_MAX_WIDTH_TYPE_STR ((int)strlen(MAX_WIDTH_TYPE_STR))

#define INDENT_SPACES 2

long clock_to_msec(clock_t c)
{
    return (((double)c) * 1000) / CLOCKS_PER_SEC;
}

void print_results(const char* type, size_t nelements, clock_t begin,
    clock_t end)
{
    printf("%*s%-*s : %10zu elements | %5ld msec\n",
        INDENT_SPACES,
        "", /* for indent %*s */
        WIDTH_OF_MAX_WIDTH_TYPE_STR,
        type, nelements,
        clock_to_msec(end-begin));
}

void fill_pre_sized(void);
void fill_push_back(void);
void insert_front(void);
void insert_rand(void);
void remove_front(void);
void remove_rand(void);
void swap_rand(void);

int main(void)
{
    time_t seed = time(NULL);
    puts(HR40 HR40);
    printf("seed: %ld\n\n", seed);
    srand(seed);
    fill_pre_sized(); putchar('\n');
    fill_push_back(); putchar('\n');
    insert_front();   putchar('\n');
    insert_rand();    putchar('\n');
    remove_front();   putchar('\n');
    remove_rand();    putchar('\n');
    swap_rand();
    puts(HR40 HR40);
    return EXIT_SUCCESS;
}

clock_t begin;
clock_t end;
const int init_elem = 1;
