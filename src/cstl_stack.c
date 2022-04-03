#include "cstl_stack.h"

struct stack_struct
{
    Vector *vec;
    void *(*mem_alloc)(size_t size);
    void *(*mem_calloc)(size_t blocks, size_t size);
    void (*mem_free)(void *block);
};

void cstl_stack_hepler_init(Stack_Helper *hepler)
{
    cstl_vector_hepler_init(hepler);
}

State cstl_stack_create(Stack **out)
{
    Stack_Helper helper;
    cstl_stack_hepler_init(&helper);
    return cstl_stack_helper_create(&helper, out);
}

State cstl_stack_helper_create(const Stack_Helper *const hepler, Stack **out)
{
    Stack *stack = hepler->mem_calloc(1, sizeof(Stack));
    if (!stack)
        return CSTL_ERR_ALLOC;

    stack->mem_alloc = hepler->mem_alloc;
    stack->mem_calloc = hepler->mem_calloc;
    stack->mem_free = hepler->mem_free;

    Vector *vec;
    State state;
    if ((state = cstl_vector_create_helper(hepler, &vec)) == CSTL_OK)
    {
        stack->vec = vec;
    }
    else
    {
        hepler->mem_free(stack);
        return state;
    }
    *out = stack;
    return CSTL_OK;
}

void cstl_stack_destroy(Stack *stack)
{
    cstl_vector_destory(stack->vec);
    stack->mem_free(stack);
}

void cstl_stack_destroy_cb(Stack *stack, void (*cb)(void *))
{
    cstl_vector_destory_cb(stack->vec, cb);
    free(stack);
}

State cstl_stack_push(Stack *stack, void *element)
{
    return cstl_vector_push_back(stack->vec, element);
}

void *cstl_stack_peek(Stack *stack)
{
    return cstl_vector_back(stack->vec);
}

void *cstl_stack_pop(Stack *stack)
{
    return cstl_vector_pop_back(stack->vec);
}

size_t cstl_stack_size(Stack *stack)
{
    return cstl_vector_size(stack->vec);
}

bool cstl_stack_empty(Stack *stack)
{
    return cstl_stack_size(stack) == 0;
}
