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

    memcpy(new_buffer, vec->buffer, vec->size * sizeof(void *));

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
State cstl_vector_push_back(Vector *vec, void *element)
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
State cstl_vector_push_at(Vector *vec, void *element, size_t index)
{
    if (index == vec->size)
        return cstl_vector_push_back(vec, element);

    if ((vec->size == 0 && index != 0) || index > vec->size)
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
    if (index >= vec->size)
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
    if (index1 >= vec->size || index2 >= vec->size)
        return CSTL_ERR_OUT_OF_RANGE;

    tmp = vec->buffer[index1];
    vec->buffer[index1] = vec->buffer[index2];
    vec->buffer[index2] = tmp;
    return CSTL_OK;
}

/**
 * @brief 获取Vector中位于index位置的元素并返回其值。index应位于数组范围之内
 *
 * @param vec
 * @param index
 * @return void* 指向元素的指针
 */
void *cstl_vector_at(Vector *vec, size_t index)
{
    if (index >= vec->size)
        return NULL;
    return vec->buffer[index];
}

/**
 * @brief 获取Vector的最后一个元素
 *
 * @param vec
 * @return void** 指向元素的指针
 */
void *cstl_vector_back(Vector *vec)
{
    if (vec->size == 0)
        return NULL;
    return cstl_vector_at(vec, vec->size - 1);
}

/**
 * @brief 获取Vector中第一次出现element元素的索引位置
 *
 * @param vec
 * @param element
 * @return size_t* 指向索引的指针
 */
size_t *cstl_vector_index_of(Vector *vec, void *element)
{
    size_t *res = NULL;
    for (size_t i = 0; i < vec->size; ++i)
    {
        if (vec->buffer[i] == element)
        {
            *res = i;
            break;
        }
    }
    return res;
}

/**
 * @brief 移除指定元素，并返回
 *
 * @param vec
 * @param element
 * @return void*
 */
void *cstl_vector_remove(Vector *vec, void *element)
{
    size_t *index = cstl_vector_index_of(vec, element);
    if (index == NULL)
        return NULL;
    if (*index != vec->size - 1)
    {
        size_t block_size = (vec->size - 1 - *index) * sizeof(void *);
        memmove(&(vec->buffer[*index]), &(vec->buffer[*index + 1]), block_size);
    }
    --(vec->size);
    return element;
}

/**
 * @brief 删除指定索引位置处的元素
 *
 * @param vec
 * @param index
 * @return void*
 */
void *cstl_vector_remove_at(Vector *vec, size_t index)
{
    if (index >= vec->size)
        return NULL;
    void *res = vec->buffer[index];
    if (index != vec->size - 1)
    {
        size_t block_size = (vec->size - 1 - index) * sizeof(void *);
        memmove(&(vec->buffer[index]), &(vec->buffer[index + 1]), block_size);
    }
    --(vec->size);
    return res;
}

/**
 * @brief 删除数组最后一个元素
 *
 * @param vec
 * @return void*
 */
void *cstl_vector_pop_back(Vector *vec)
{
    return cstl_vector_remove_at(vec, vec->size - 1);
}

/**
 * @brief 移除数组中所有元素，该函数不需要删除buffer中的元素和减小capacity
 *
 * @param vec
 * @return State
 */
void cstl_vector_remove_all(Vector *vec)
{
    vec->size = 0;
}

/**
 * @brief 移除数组中所有元素并删除buffer中的元素，但不需要减小capacity
 *
 * @param vec
 */
void cstl_vector_remove_all_free(Vector *vec)
{
    size_t i;
    for (i = 0; i < vec->size; ++i)
        vec->mem_free(vec->buffer[i]);
    cstl_vector_remove_all(vec);
}

/**
 * @brief 返回Vector的size
 *
 * @param vec
 * @return size_t
 */
size_t cstl_vector_size(Vector *vec)
{
    return vec->size;
}

/**
 * @brief 返回Vector的capacity
 *
 * @param vec
 * @return size_t
 */
size_t cstl_vector_capacity(Vector *vec)
{
    return vec->capacity;
}
