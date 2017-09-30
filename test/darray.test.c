#if __linux__
#   define _EMU_ENABLE_COLOR_
#endif
#include <EMUtest.h>
#include <time.h>
#include "../darray.h"

#define INITIAL_NUM_ELEMS 10
#define RESIZE_NUM_ELEMS 100

EMU_TEST(alloc_and_free_functions)
{
    int* da = da_alloc(INITIAL_NUM_ELEMS, sizeof(int));
    EMU_REQUIRE_NOT_NULL(da);
    for (size_t i = 0; i < INITIAL_NUM_ELEMS; ++i) // writiable without crashing
    {
        da[i] = i;
    }

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_length)
{
    int* da = da_alloc(INITIAL_NUM_ELEMS, sizeof(int));

    EMU_EXPECT_EQ_UINT(da_length(da), INITIAL_NUM_ELEMS);
    EMU_PRINT_INDENT();
    printf("length  : %zu\n", da_length(da));

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_capacity)
{
    int* da = da_alloc(INITIAL_NUM_ELEMS, sizeof(int));

    EMU_EXPECT_GE_UINT(da_capacity(da), INITIAL_NUM_ELEMS);
    EMU_PRINT_INDENT();
    printf("capacity : %zu\n", da_capacity(da));

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_sizeof_elem)
{
    int*   da1 = da_alloc(INITIAL_NUM_ELEMS, sizeof(int));
    void*  da2 = da_alloc(INITIAL_NUM_ELEMS, sizeof(int));
    float* da3 = da_alloc(INITIAL_NUM_ELEMS, sizeof(float));

    EMU_EXPECT_EQ_UINT(da_sizeof_elem(da1), sizeof(int));
    EMU_EXPECT_EQ_UINT(da_sizeof_elem(da2), sizeof(int));
    EMU_EXPECT_EQ_UINT(da_sizeof_elem(da3), sizeof(float));

    da_free(da1);
    da_free(da2);
    da_free(da3);

    EMU_END_TEST();
}

EMU_TEST(da_resize)
{
    int* da = da_alloc(INITIAL_NUM_ELEMS, sizeof(int));
    for (size_t i = 0; i < INITIAL_NUM_ELEMS; ++i)
    {
        da[i] = i;
    }

    da = da_resize(da, RESIZE_NUM_ELEMS);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_EXPECT_EQ_UINT(da_length(da), RESIZE_NUM_ELEMS);
    for (size_t i = 0; i < INITIAL_NUM_ELEMS; ++i)
    {
        EMU_EXPECT_EQ_INT(da[i], (int)i);
    }

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_reserve)
{
    int* da = da_alloc(INITIAL_NUM_ELEMS, sizeof(int));

    da = da_reserve(da, 50);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_EXPECT_GE_UINT(da_capacity(da)-da_length(da), 50);

    // multiple calls to reserve with the same reserve nelem should only cause
    // the array to change the first time.
    int* da2 = da_reserve(da, 50);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_EXPECT_EQ(da2, da);

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_push)
{
    const int max_index = 15;
    int* da = da_alloc(0, sizeof(int));

    for (int i = 0; i <= max_index; ++i)
    {
        da_push(da, i);
    }
    EMU_REQUIRE_NOT_NULL(da);
    EMU_EXPECT_EQ_UINT(da_length(da), max_index+1);
    for (int i = 0; i <= max_index; ++i)
    {
        EMU_EXPECT_EQ_INT(da[i], i);
    }

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_spush)
{
    const int max_index = 15;
    int* da = da_alloc(0, sizeof(int));

    for (int i = 0; i <= max_index; ++i)
    {
        da = da_spush(da, &i);
        EMU_REQUIRE_NOT_NULL(da);
    }
    EMU_EXPECT_EQ_UINT(da_length(da), max_index+1);
    for (int i = 0; i <= max_index; ++i)
    {
        EMU_EXPECT_EQ_INT(da[i], i);
    }

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_pop)
{
    int* da = da_alloc(2, sizeof(int));
    da[0] = 3;
    da[1] = 5;

    EMU_EXPECT_EQ_INT(da_pop(da), 5);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_EXPECT_EQ_UINT(da_length(da), 1);

    EMU_EXPECT_EQ_INT(da_pop(da), 3);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_EXPECT_EQ_UINT(da_length(da), 0);

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_insert)
{
    int* da = da_alloc(2, sizeof(int));
    da[0] = 3;
    da[1] = 5;

    da_insert(da, 0, 7);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_EXPECT_EQ_UINT(da_length(da), 3);
    EMU_EXPECT_EQ_INT(da[0], 7);
    EMU_EXPECT_EQ_INT(da[1], 3);
    EMU_EXPECT_EQ_INT(da[2], 5);

    da_insert(da, 1, 9);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_EXPECT_EQ_UINT(da_length(da), 4);
    EMU_EXPECT_EQ_INT(da[0], 7);
    EMU_EXPECT_EQ_INT(da[1], 9);
    EMU_EXPECT_EQ_INT(da[2], 3);
    EMU_EXPECT_EQ_INT(da[3], 5);

    da_free(da);

    da = da_alloc(0, sizeof(int));
    const int max_index = 15;

    // mimic push front
    for (int i = max_index; i >= 0; --i)
    {
        da_insert(da, 0, i);
        EMU_REQUIRE_NOT_NULL(da);
    }
    EMU_EXPECT_EQ_UINT(da_length(da), max_index+1);
    for (int i = max_index; i >= 0; --i)
    {
        EMU_EXPECT_EQ_INT(da[i], i);
    }

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_sinsert)
{
    int* da = da_alloc(2, sizeof(int));
    da[0] = 3;
    da[1] = 5;

    int val = 7;
    da = da_sinsert(da, 0, &val);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_EXPECT_EQ_UINT(da_length(da), 3);
    EMU_EXPECT_EQ_INT(da[0], 7);
    EMU_EXPECT_EQ_INT(da[1], 3);
    EMU_EXPECT_EQ_INT(da[2], 5);

    val = 9;
    da = da_sinsert(da, 1, &val);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_EXPECT_EQ_UINT(da_length(da), 4);
    EMU_EXPECT_EQ_INT(da[0], 7);
    EMU_EXPECT_EQ_INT(da[1], 9);
    EMU_EXPECT_EQ_INT(da[2], 3);
    EMU_EXPECT_EQ_INT(da[3], 5);

    da_free(da);

    da = da_alloc(0, sizeof(int));
    const int max_index = 15;

    // mimic push front
    for (int i = max_index; i >= 0; --i)
    {
        da_sinsert(da, 0, &i);
        EMU_REQUIRE_NOT_NULL(da);
    }
    EMU_EXPECT_EQ_UINT(da_length(da), max_index+1);
    for (int i = max_index; i >= 0; --i)
    {
        EMU_EXPECT_EQ_INT(da[i], i);
    }

    da_free(da);
    EMU_END_TEST();
}

struct bigstruct {int A[500];};

EMU_TEST(da_remove)
{
    int* da = da_alloc(4, sizeof(int));
    da[0] = 3;
    da[1] = 5;
    da[2] = 7;
    da[3] = 9;

    // remove from middle
    EMU_EXPECT_EQ_INT(da_remove(da, 1), 5);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_EXPECT_EQ_UINT(da_length(da), 3);

    // remove from front
    EMU_EXPECT_EQ_INT(da_remove(da, 0), 3);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_EXPECT_EQ_UINT(da_length(da), 2);

    // remove from back
    EMU_EXPECT_EQ_INT(da_remove(da, 1), 9);
    EMU_REQUIRE_NOT_NULL(da);
    EMU_EXPECT_EQ_UINT(da_length(da), 1);

    struct bigstruct* bda = da_alloc(4, sizeof(struct bigstruct));
    da_remove(bda, 1); // remove from middle
    da_remove(bda, 0); // remove from front
    da_remove(bda, 1); // remove from back


    da_free(da);
    da_free(bda);
    EMU_END_TEST();
}

EMU_TEST(da_swap)
{
    int* da = da_alloc(INITIAL_NUM_ELEMS, sizeof(int));

    da[3] = 12;
    da[5] = 99;

    da_swap(da, 3, 5);
    EMU_EXPECT_EQ_INT(da[3], 99);
    EMU_EXPECT_EQ_INT(da[5], 12);

    da_swap(da, 3, 5);
    EMU_EXPECT_EQ_INT(da[3], 12);
    EMU_EXPECT_EQ_INT(da[5], 99);

    // swap element with itself
    da_swap(da, 3, 3);
    EMU_EXPECT_EQ_INT(da[3], 12);
    EMU_EXPECT_EQ_INT(da[5], 99);

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_cat)
{
    char* A = da_alloc(1, sizeof(char));
    long* B = da_alloc(1, sizeof(long));
    long* C = da_cat(B, A);
    EMU_EXPECT_NULL(C);
    EMU_EXPECT_NOT_NULL(A);
    EMU_EXPECT_NOT_NULL(B);
    da_free(A);
    da_free(B);

    int* src = da_alloc(2, sizeof(int));
    int* dest = da_alloc(3, sizeof(int));

    dest[0] = 0;
    dest[1] = 1;
    dest[2] = 2;

    src[0] = 3;
    src[1] = 4;

    dest = da_cat(dest, src);
    EMU_REQUIRE_NOT_NULL(dest);
    EMU_REQUIRE_NOT_NULL(src);
    for (size_t i = 0; i < 5; ++i)
    {
        EMU_EXPECT_EQ_INT(dest[i], i);
    }

    da_free(src);
    da_free(dest);
    EMU_END_TEST(); 
}

EMU_TEST(da_fill)
{
    int* da = da_alloc(INITIAL_NUM_ELEMS, sizeof(int));

    for (size_t i = 0; i < da_length(da); ++i)
    {
        da[i] = i;
    }

    da_fill(da, int, 12 + 3);
    for (size_t i = 0; i < da_length(da); ++i)
    {
        EMU_EXPECT_EQ_INT(da[i], 15);
    }

    da_fill(da, int, rand());
    for (size_t i = 1; i < da_length(da); ++i)
    {
        EMU_EXPECT_EQ_INT(da[i], da[i-1]);
    }

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_foreach)
{
    int* da = da_alloc(INITIAL_NUM_ELEMS, sizeof(int));

    // test general iteration through all elements
    for (size_t i = 0; i < da_length(da); ++i){da[i] = i;}
    da_foreach(da, int, iter)
    {
        *iter += 1;
    }
    for (size_t i = 0; i < da_length(da); ++i)
    {
        EMU_EXPECT_EQ_INT(da[i], i + 1);
    }

    // test forward iteration
    for (size_t i = 0; i < da_length(da); ++i){da[i] = 0;}
    da_foreach(da, int, iter)
    {
        if (iter != da)
            *iter = *(iter-1) + 1;
    }
    for (size_t i = 1; i < da_length(da); ++i)
    {
        EMU_EXPECT_EQ_INT(da[i], da[i-1] + 1);
    }

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(da_foreachr)
{
    int* da = da_alloc(INITIAL_NUM_ELEMS, sizeof(int));

    // test general iteration through all elements
    for (size_t i = 0; i < da_length(da); ++i){da[i] = i;}
    da_foreachr(da, int, iter)
    {
        *iter += 1;
    }
    for (size_t i = 0; i < da_length(da); ++i)
    {
        EMU_EXPECT_EQ_INT(da[i], i + 1);
    }

    // test reverse iteration
    for (size_t i = 0; i < da_length(da); ++i){da[i] = 0;}
    da_foreachr(da, int, iter)
    {
        if (iter != da + da_length(da))
            *iter = *(iter+1) + 1;
    }
    for (int i = da_length(da)-1; i >= 0; --i)
    {
        EMU_EXPECT_EQ_INT(da[i], da[i+1] + 1);
    }

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(container_style_type)
{
    int* da = da_alloc(INITIAL_NUM_ELEMS, sizeof(int));
    darray(int) da2 = da; // check for no comiler warnings

    da_free(da2);
    EMU_END_TEST();
}

EMU_GROUP(darray_functions)
{
    EMU_ADD(alloc_and_free_functions);
    EMU_ADD(da_length);
    EMU_ADD(da_capacity);
    EMU_ADD(da_sizeof_elem);
    EMU_ADD(da_resize);
    EMU_ADD(da_reserve);
    EMU_ADD(da_push);
    EMU_ADD(da_spush);
    EMU_ADD(da_pop);
    EMU_ADD(da_insert);
    EMU_ADD(da_sinsert);
    EMU_ADD(da_remove);
    EMU_ADD(da_swap);
    EMU_ADD(da_cat);
    EMU_ADD(da_fill);
    EMU_ADD(da_foreach);
    EMU_ADD(da_foreachr);
    EMU_ADD(container_style_type);
    EMU_END_GROUP();
}

struct foo
{
    int a;
    char b;
    double c;
};

EMU_TEST(small_struct)
{
    struct foo* da = da_alloc(2, sizeof(struct foo));
    EMU_REQUIRE_NOT_NULL(da);

    struct foo some_struct = {.a=3, .b='y', .c=3.14159};
    da[0] = some_struct;
    // da_push(da, (struct foo){.a=4, .b='z', .c=4.14159});
    // ^^^ this will cause a compile time error since the commas in the
    // declaration get parsed as separate arguments to the push macro.
    da_push(da, some_struct);

    struct foo bar = da_remove(da, 0);
    EMU_EXPECT_EQ(some_struct.a, bar.a);
    EMU_EXPECT_EQ(some_struct.b, bar.b);
    EMU_EXPECT_EQ(some_struct.c, bar.c);

    da_free(da);
    EMU_END_TEST();
}

EMU_TEST(cstrings)
{
    char* da = da_alloc(strlen("some string")+1, 1);
    memcpy(da, "some string", da_length(da));
    EMU_PRINT_INDENT(); printf("%s\n", da);
    EMU_END_TEST();
}

EMU_GROUP(testing_with_different_types)
{
    EMU_ADD(small_struct);
    EMU_ADD(cstrings);
    EMU_END_GROUP();
}

EMU_GROUP(all_tests)
{
    EMU_ADD(darray_functions);
    EMU_ADD(testing_with_different_types);
    EMU_END_GROUP();
}

int main(void)
{
    time_t seed = time(NULL);
    printf("seed: %ld\n", seed);
    srand(seed);
    return EMU_RUN(all_tests);
}
