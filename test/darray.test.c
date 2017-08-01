#if __linux__
#   define _EMU_ENABLE_COLOR_
#endif
#include <EMUtest.h>
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
        EMU_EXPECT_EQ_INT(da[i], i);
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

EMU_TEST(da_pop)
{
    int* da = da_alloc(2, sizeof(int));
    da[0] = 3;
    da[1] = 5;

    EMU_EXPECT_EQ_INT(da_pop(da), 5);
    EMU_EXPECT_EQ_UINT(da_length(da), 1);

    EMU_EXPECT_EQ_INT(da_pop(da), 3);
    EMU_EXPECT_EQ_UINT(da_length(da), 0);

    da_free(da);
    EMU_END_TEST();
}

EMU_GROUP(all_tests)
{
    EMU_ADD(alloc_and_free_functions);
    EMU_ADD(da_length);
    EMU_ADD(da_capacity);
    EMU_ADD(da_sizeof_elem);
    EMU_ADD(da_resize);
    EMU_ADD(da_reserve);
    EMU_ADD(da_push);
    EMU_ADD(da_pop);
    EMU_END_GROUP();
}

int main(void)
{
    return EMU_RUN(all_tests);
}
