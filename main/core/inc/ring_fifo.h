#ifndef __RING_FIFO_H__
#define __RING_FIFO_H__

#include <stdint.h>
#include <stdlib.h>

#ifndef NUM_BITS
#define NUM_BITS 16
#endif

#if (NUM_BITS == 16)
#define NUM_TYPE uint16_t
#define NUM_TYPE_SIGNED int16_t
#elif (NUM_BITS == 32)
#define NUM_TYPE uint32_t
#define NUM_TYPE_SIGNED int32_t
#elif (NUM_BITS == 64)
#define NUM_TYPE uint64_t
#define NUM_TYPE_SIGNED int64_t
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *buffer;            // 数据块地址
    NUM_TYPE capacity;       // 数据最大存放个数
    NUM_TYPE element_size;   // 单个数据占用的字节数
    NUM_TYPE cover;          // 是否支持覆盖
    volatile NUM_TYPE head;  // 起始下标
    volatile NUM_TYPE tail;  // 结尾下标
    volatile NUM_TYPE size;  // 数据实际个数
} RING_FIFO;

/**
 * @brief  定义 RING_FIFO 变量
 * @param  Type 存放的数据类型
 * @param  BufName RING_FIFO 变量名称
 * @param  Size 最多可以存放的数据数量
 * @param  Cover 是否支持覆盖 (0 / 1)
 * @retval None
 */
#define ring_define(Type, BufName, Size, Cover)        \
    Type __##BufName##_data[Size];                     \
    RING_FIFO BufName = {                              \
        .buffer = __##BufName##_data,                  \
        .capacity = Size,                              \
        .element_size = sizeof(__##BufName##_data[0]), \
        .cover = Cover,                                \
        .head = 0,                                     \
        .tail = 0,                                     \
        .size = 0,                                     \
    }

/**
 * @brief  定义 RING_FIFO 静态变量
 * @param  Type 存放的数据类型
 * @param  BufName RING_FIFO 变量名称
 * @param  Size 最多可以存放的数据数量
 * @param  Cover 是否支持覆盖 (0 / 1)
 * @retval None
 */
#define ring_define_static(Type, BufName, Size, Cover) \
    static Type __##BufName##_data[Size];              \
    static RING_FIFO BufName = {                       \
        .buffer = __##BufName##_data,                  \
        .capacity = Size,                              \
        .element_size = sizeof(__##BufName##_data[0]), \
        .cover = Cover,                                \
        .head = 0,                                     \
        .tail = 0,                                     \
        .size = 0,                                     \
    }

/**
 * @brief  放入单个数据
 * @param  ring RING_FIFO 变量的地址
 * @param  element 待存入数据的地址
 * @retval 是否成功放入，失败时返回值小于 0（支持覆盖时，参数正确传入则永远返回0）
 */
int8_t ring_push(RING_FIFO *ring, const void *element);

/**
 * @brief  优先队列，二分插入（仅在非覆盖模式下可用）
 * @param  ring RING_FIFO 变量的地址
 * @param  element 待存入数据的地址
 * @param  compare 比较 element 的函数，返回值 [-1, 0, 1]
 * @retval 是否成功放入，失败时返回值小于 0
 */
int8_t ring_binsert(RING_FIFO *ring, const void *element, int (*compare)(const void *, const void *));

/**
 * @brief  取出单个数据
 * @param  ring RING_FIFO 变量的地址
 * @param  element 取出数据的存放地址
 * @retval 是否成功取出，失败时返回值小于 0
 */
int8_t ring_pop(RING_FIFO *ring, void *element);

/**
 * @brief  弹出单个数据
 * @param  ring RING_FIFO 变量的地址
 * @retval 是否成功弹出，失败时返回值小于 0
 */
int8_t ring_pop_unread(RING_FIFO *ring);

/**
 * @brief  弹出多个数据
 * @param  ring RING_FIFO 变量的地址
 * @param  num 弹出数据的个数
 * @retval 成功弹出几个
 */
NUM_TYPE ring_pop_mult_unread(RING_FIFO *ring, NUM_TYPE num);

/**
 * @brief  获取下一个被取出数据的地址
 * @param  ring RING_FIFO 变量的地址
 * @retval 成功时返回数据地址，否则返回 NULL
 */
void *ring_peek(RING_FIFO *ring);

/**
 * @brief  获取当前数据地址的下一个数据地址
 * @param  ring RING_FIFO 变量的地址
 * @param  ptr 当前数据地址
 * @retval ptr 传入 NULL 时返回数据首地址，有下一个数据时返回下一个数据地址，否则返回 NULL
 */
void *ring_peek_next(RING_FIFO *ring, void *ptr);

/**
 * @brief  放入多个数据
 * @param  ring RING_FIFO 变量的地址
 * @param  elements 待存入数据块的首地址
 * @param  num 待存入数据的个数
 * @retval 成功放入几个（支持覆盖时，参数正确传入则返回值与数据个数相等）
 */
NUM_TYPE ring_push_mult(RING_FIFO *ring, const void *elements, NUM_TYPE num);

/**
 * @brief  取出多个数据
 * @param  ring RING_FIFO 变量的地址
 * @param  elements 取出数据块的存放地址
 * @param  num 待取出数据的个数
 * @retval 成功取出几个
 */
NUM_TYPE ring_pop_mult(RING_FIFO *ring, void *elements, NUM_TYPE num);

/**
 * @brief  深拷贝 RING_FIFO
 * @param  ring_dst 目标 RING_FIFO 变量的地址
 * @param  ring_src 源 RING_FIFO 变量的地址
 * @retval 成功返回 0，否则返回 -1
 */
int8_t ring_deep_copy(RING_FIFO *ring_dst, RING_FIFO *ring_src);

/**
 * @brief  重置 RING_FIFO
 * @param  ring RING_FIFO 变量的地址
 * @retval None
 */
void ring_reset(RING_FIFO *ring);

/**
 * @brief  RING_FIFO 是否为空
 * @param  ring RING_FIFO 变量的地址
 * @retval 为空返回 1
 */
int8_t ring_is_empty(RING_FIFO *ring);

/**
 * @brief  RING_FIFO 是否已满
 * @param  ring RING_FIFO 变量的地址
 * @retval 已满返回 1
 */
int8_t ring_is_full(RING_FIFO *ring);

/**
 * @brief  获取 RING_FIFO 内实际数据数量
 * @param  ring RING_FIFO 变量的地址
 * @retval RING_FIFO 内实际数据数量
 */
NUM_TYPE ring_size(RING_FIFO *ring);

/**
 * @brief  打印 RING_FIFO 内部信息
 * @param  ring RING_FIFO 变量的地址
 * @retval None
 */
void print_ring(RING_FIFO *ring);

#ifdef __cplusplus
}
#endif

#endif
