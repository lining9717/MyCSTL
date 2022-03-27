#ifndef MY_CSTL_VECTOR_H
#define MY_CSTL_VECTOR_H

#include "cstl_common.h"

#define DEFAULT_CAPACITY 8
#define DEFAULT_EXPANSION_RATE 2

/**
 * @brief 可自动扩增的动态数组
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

State cstl_vector_create(Vector **out);
void cstl_vector_hepler_init(Vector_Hepler *vh);
State cstl_vector_create_helper(const Vector_Hepler *const vh, Vector **out);

void cstl_vector_destory(Vector *vec);
void cstl_vector_destory_cb(Vector *vec, void (*cb)(void *));

State cstl_vector_add(Vector *vec, void *element);
State cstl_vector_add_at(Vector *vec, void *element, size_t index);
State cstl_vector_replace_at(Vector *vec, void *element, size_t index, void **out);
State cstl_vector_swap_at(Vector *vec, size_t index1, size_t idnex2);

State cstl_vector_remove(Vector *vec, void *element, void **out);
State cstl_vector_remove_at(Vector *vec, size_t index, void **out);
State cstl_vector_remove_last(Vector *vec, void **out);
State cstl_vector_remove_all(Vector *vec);
State cstl_vector_remove_all_free(Vector *vec);

#endif