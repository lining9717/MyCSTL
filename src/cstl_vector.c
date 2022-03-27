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

/**
 * @brief 扩展Vector的容量。当新的内存分配失败时扩展失败。
 * 可能的原因有：
 * 1、CSTL_ERR_MAX_CAPACITY：容量超出最大值
 * 2、CSTL_ERR_ALLOC：新内存块分配失败
 *
 * @param vec 需要扩展容量的vector
 * @return State CSTL_OK代表扩展内存成功；CSTL_ERR_ALLOC代表分配新内存失败；CSTL_ERR_MAX_CAPACITY代表容量超出最大值
 */
static State expand_capacity(Vector *vec)
{
    if (vec->capacity >= CSTL_MAX_ELEMENTS)
        return CSTL_ERR_MAX_CAPACITY;
    size_t new_capacity = vec->capacity * vec->expansion_rate;

    if (new_capacity <= vec->capacity)
        vec->capacity = CSTL_MAX_ELEMENTS;
    else
        vec->capacity = new_capacity;
    void **new_buffer = vec->mem_alloc(new_capacity * sizeof(void *));
    if (!new_buffer)
        return CSTL_ERR_ALLOC;

    memcpy_s(new_buffer, new_capacity * sizeof(void *),
             vec->buffer, vec->size * sizeof(void *));

    vec->mem_free(vec->buffer);
    vec->buffer = new_buffer;

    return CSTL_OK;
}

/**
 * @brief 创建新的空数组并返回状态码
 *
 * @param out 指向新创建的数组的指针
 * @return State CSTL_OK代表成功，CSTL_ERR_ALLOC代表内存分配失败
 */
State cstl_vector_create(Vector **out)
{
    Vector_Hepler vh;
    cstl_vector_hepler_init(&vh);
    return cstl_vector_create_helper(&vh, out);
}

/**
 * @brief 初始化Vector_Hepler为默认值
 *
 * @param vh
 */
void cstl_vector_hepler_init(Vector_Hepler *vh)
{
    vh->capacity = DEFAULT_CAPACITY;
    vh->expansion_rate = DEFAULT_EXPANSION_RATE;
    vh->mem_alloc = malloc;
    vh->mem_calloc = calloc;
    vh->mem_free = free;
}

/**
 * @brief 通过Vector_Hepler创建新的空Vector并返回状态码
 *
 * Vector通过Vector_Hepler实现内存分配。内存分配可能出错，
 * 其中一种CSTL_ERR_INVALID_CAPACITY，即ex_rate >= CSTL_MAX_ELEMENTS / vh->capacity
 *
 * @param vh Vector构造辅助结构
 * @param out 指向新创建Vector的指针
 * @return State CSTL_OK代表成功，CSTL_ERR_ALLOC代表内存分配错误，CSTL_ERR_INVALID_CAPACITY如上所述
 */
State cstl_vector_create_helper(const Vector_Hepler *const vh, Vector **out)
{
    float ex_rate;
    if (vh->expansion_rate <= 1)
        ex_rate = DEFAULT_EXPANSION_RATE;
    else
        ex_rate = vh->expansion_rate;

    // 需要避免数组扩张之后大于整数最大值
    if (!vh->capacity || ex_rate >= CSTL_MAX_ELEMENTS / vh->capacity)
        return CSTL_ERR_INVALID_CAPACITY;

    Vector *vec = vh->mem_calloc(1, sizeof(Vector));

    if (!vec)
        return CSTL_ERR_ALLOC;

    void **buffer = vh->mem_alloc(vh->capacity * (sizeof(void *)));

    if (!buffer)
    {
        vh->mem_free(vec);
        return CSTL_ERR_ALLOC;
    }

    vec->buffer = buffer;
    vec->expansion_rate = ex_rate;
    vec->capacity = vh->capacity;
    vec->mem_alloc = vh->mem_alloc;
    vec->mem_calloc = vh->mem_calloc;
    vec->mem_free = vh->mem_free;

    *out = vec;
    return CSTL_OK;
}

/**
 * @brief 销毁Vector结构
 *
 * @param vec 用于销毁的Vector
 */
void cstl_vector_destory(Vector *vec)
{
    vec->mem_free(vec->buffer);
    vec->mem_free(vec);
}

/**
 * @brief 销毁Vector结构和其中所包含的数据
 *
 * 该函数在vector分配在栈上时不能被调用
 *
 * @param vec 用于销毁的Vector
 * @param cb 用于销毁的Vector中的元素函数
 */
void cstl_vector_destory_cb(Vector *vec, void (*cb)(void *))
{
    size_t i;
    for (int i = 0; i < vec->size; ++i)
        cb(vec->buffer[i]);

    cstl_vector_destory(vec);
}

/**
 * @brief 向Vector的末尾添加一个新的元素
 *
 * @param vec 需要添加元素的数组
 * @param element 需要被添加的元素
 * @return State CSTL_OK代表添加元素成功；
 *               CSTL_ERR_ALLOC代表分配新内存失败；
 *               CSTL_ERR_MAX_CAPACITY代表容量超出最大值
 */
State cstl_vector_add(Vector *vec, void *element)
{
    if (vec->size >= vec->capacity)
    {
        State sta = expand_capacity(vec);
        if (sta != CSTL_OK)
            return sta;
    }
    vec->buffer[vec->size] = element;
    ++(vec->size);

    return CSTL_OK;
}

/**
 * @brief 在数组Vector的特定位置增加一个元素。该元素的位置必须位于数组大小之内
 * 若数组容量不足，则扩展数组，之后移动插入位置之后的元素
 *
 * @param vec 需要被插入元素的Vector
 * @param element 插入的元素
 * @param index 插入元素的索引
 * @return State CSTL_OK代表添加元素成功；
 *               CSTL_ERR_ALLOC代表分配新内存失败；
 *               CSTL_ERR_MAX_CAPACITY代表容量超出最大值；
 *               CSTL_ERR_OUT_OF_RANGE代表插入元素的位置不在范围之内
 */
State cstl_vector_add_at(Vector *vec, void *element, size_t index)
{
    if (index == vec->size)
        return cstl_vector_add(vec, element);

    if ((vec->size == 0 && index != 0) || index < 0 || index > vec->size)
        return CSTL_ERR_OUT_OF_RANGE;

    if (vec->size >= vec->capacity)
    {
        State sta = expand_capacity(vec);
        if (sta != CSTL_OK)
            return sta;
    }

    size_t shift_size = (vec->size - index) * sizeof(void *);
    memmove(&(vec->buffer[index + 1]), &(vec->buffer[index]), shift_size);

    vec->buffer[index] = element;
    ++(vec->size);

    return CSTL_OK;
}

/**
 * @brief 用于替换Vector中的特定元素，索引在数组范围之内；将被替换的元素输出值out
 *
 * @param vec 需要被替换元素的Vector
 * @param element  新元素
 * @param index 替换元素的索引
 * @param out
 * @return State CSTL_OK代表替换元素成功
 *               CSTL_ERR_OUT_OF_RANGE代表替换元素的位置不在范围之内
 */
State cstl_vector_replace_at(Vector *vec, void *element, size_t index, void **out)
{
    if (index < 0 || index >= vec->size)
        return CSTL_ERR_OUT_OF_RANGE;
    if (out)
        *out = vec->buffer[index];
    vec->buffer[index] = element;
    return CSTL_OK;
}

/**
 * @brief 交换Vector中两个特定位置的值
 *
 * @param vec
 * @param index1
 * @param index2
 * @return State
 */
State cstl_vector_swap_at(Vector *vec, size_t index1, size_t index2)
{
    void *tmp;
    if (index1 < 0 || index1 >= vec->size || index2 < 0 || index2 >= vec->size)
        return CSTL_ERR_OUT_OF_RANGE;

    tmp = vec->buffer[index1];
    vec->buffer[index1] = vec->buffer[index2];
    vec->buffer[index2] = tmp;
    return CSTL_OK;
}

State cstl_vector_remove(Vector *vec, void *element, void **out)
{

}

State cstl_vector_remove_at(Vector *vec, size_t index, void **out)
{
}
State cstl_vector_remove_last(Vector *vec, void **out)
{
}
State cstl_vector_remove_all(Vector *vec)
{
}
State cstl_vector_remove_all_free(Vector *vec)
{
}