#include <stdio.h>
#include "cstl_vector.h"

int main()
{
    Vector *v1;
    cstl_vector_create(&v1);
    printf("%10s%10s\n", "capacity", "size");
    printf("%10d%10d\n", (int)cstl_vector_capacity(v1), (int)cstl_vector_size(v1));
    return 0;
}
