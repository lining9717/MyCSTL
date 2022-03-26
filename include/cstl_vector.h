#ifndef MY_CSTL_VECTOR_H
#define MY_CSTL_VECTOR_H

#include "cstl_common.h"

#define DEFAULT_CAPACITY 8
#define DEFAULT_EXPANSION_RATE 2

/**
 * @brief 可自动扩增的动态数组。
 * 
 */
typedef struct cstl_vector Vector;

/**
 * @brief 用于数组实现的辅助结构
 * 
 */
typedef struct cstl_vector_helper
{
    // 数组的初始容量
    size_t capacity;

    // buffer的扩增速度(capacity * expansion_rate)
    float expansion_rate;

    // 内存分配器
    void *(*mem_alloc)(size_t size);
    void *(*mem_calloc)(size_t blocks, size_t size);
    void (*mem_free)(void *block);
} Vector_Hepler;



#endif