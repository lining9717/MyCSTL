#ifndef MY_CSTL_STACK_H
#define MY_CSTL_STACK_H

#include "cstl_vector.h"

/**
 * @brief 先进后出结构。包括常数时间的插入、删除和查询操作
 *
 */
typedef struct stack_struct Stack;

/**
 * @brief 用于Stack初始化
 *
 */
typedef Vector_Hepler Stack_Helper;

void cstl_stack_hepler_init(Stack_Helper *hepler);
State cstl_stack_create(Stack **out);
State cstl_stack_helper_create(const Stack_Helper *const hepler, Stack **out);

void cstl_stack_destroy(Stack *stack);
void cstl_stack_destroy_cb(Stack *stack, void (*cb)(void *));

State cstl_stack_push(Stack *stack, void *element);
void *cstl_stack_peek(Stack *stack);
void *cstl_stack_pop(Stack *stack);

size_t cstl_stack_size(Stack *stack);
bool cstl_stack_empty(Stack *stack);

#endif
