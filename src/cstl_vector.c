#include "cstl_vector.h"

struct cstl_vector
{
    size_t size;
    size_t capacity;
    float expansion_rate;
    void **buffer;

    void *(*mem_alloc)(size_t size);
    void *(*mem_calloc)(size_t blocks, size_t size);
    void (*mem_free)(void *block);
};


