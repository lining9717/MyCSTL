#ifndef MY_CSTL_COMMON_H
#define MY_CSTL_COMMON_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define CSTL_MAX_ELEMENTS ((size_t)-2) // 将-2强制转化为size_t类型的值，即最大值

enum cstl_state
{
    CSTL_OK = 0,
    CSTL_ERR_ALLOC = 1,
    CSTL_ERR_INVALID_CAPACITY = 2,
    CSTL_ERR_MAX_CAPACITY = 3,
    CSTL_ERR_OUT_OF_RANGE = 4
};

typedef enum cstl_state State;
#endif