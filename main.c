#include <stdio.h>
#include "cstl_vector.h"

void testVector()
{
    Vector *v1;
    cstl_vector_create(&v1);
    printf("%10s%10s\n", "capacity", "size");
    printf("%10d%10d\n", (int)cstl_vector_capacity(v1), (int)cstl_vector_size(v1));
    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;
    cstl_vector_push_back(v1, (void *)&a);
    cstl_vector_push_back(v1, (void *)&b);
    cstl_vector_push_back(v1, (void *)&c);
    cstl_vector_push_back(v1, (void *)&d);
    printf("%10s%10s\n", "capacity", "size");
    printf("%10d%10d\n", (int)cstl_vector_capacity(v1), (int)cstl_vector_size(v1));
    Vector_Iterator *iter;
    cstl_vector_iter_init(iter, v1);
    int *e;
    while (cstl_vector_iter_next(iter, (void *)&e) != CSTL_ITERATOR_END)
    {
        printf("%3d", *e);
    }
    printf("\n");

    if (cstl_vector_iterator_reset(iter) == CSTL_ITERATOR_NULL)
    {
        printf("iterator hasn't been initialized.\n");
        exit(1);
    }

    printf("%3d\n", *(int *)cstl_vector_at(v1, 2));
    *(int *)cstl_vector_at(v1, 2) += 3;
    printf("%3d\n", *(int *)cstl_vector_at(v1, 2));
    while (cstl_vector_iter_next(iter, (void *)&e) != CSTL_ITERATOR_END)
    {
        printf("%3d", *e);
    }
    printf("\n");

    if (cstl_vector_iterator_reset(iter) == CSTL_ITERATOR_NULL)
    {
        printf("iterator hasn't been initialized.\n");
        exit(1);
    }

    cstl_vector_pop_back(v1);
    while (cstl_vector_iter_next(iter, (void *)&e) != CSTL_ITERATOR_END)
    {
        printf("%3d", *e);
    }
    printf("\n");

    if (cstl_vector_iterator_reset(iter) == CSTL_ITERATOR_NULL)
    {
        printf("iterator hasn't been initialized.\n");
        exit(1);
    }

    cstl_vector_push_back(v1, (void *)&d);
    cstl_vector_remove_at(v1, 1);
    while (cstl_vector_iter_next(iter, (void *)&e) != CSTL_ITERATOR_END)
    {
        printf("%3d", *e);
    }
    printf("\n");
}

int main()
{
    testVector();
    return 0;
}
