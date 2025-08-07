#include "Buffer.h"


/**
 * @brief 初始化环形缓冲区 
 * @param buffer 缓冲区结构体
 * @note         头指针和尾指针都为0，清空缓冲区数据
 */
void RingBuffer_Init(Buffer_t *buffer)
{
    buffer->p_head = buffer->data;
    buffer->p_tail = buffer->data;
    memset(buffer->data, 0, sizeof(buffer->data));

#if __BUFFER_DEBUG__ == 1
    printf("RingBuffer_Init: buffer->p_head = %p, buffer->p_tail = %p\n", buffer->p_head, buffer->p_tail);
#endif
}

/**
 * @brief 写入数据
 * @param buffer 缓冲区结构体
 * @param data 数据
 * @return 状态
 */
Buffer_State_t RingBuffer_Write(Buffer_t *buffer, uint8_t *data)
{
    /* 判断缓冲区是否初始化 */
    if (buffer->p_head == NULL || buffer->p_tail == NULL)
    {
#if __BUFFER_DEBUG__ == 1
        printf("RingBuffer_Write: buffer->p_head = %p, buffer->p_tail = %p\n", buffer->p_head, buffer->p_tail);
        printf("RingBuffer_InitError\n");
#endif
        return BUFFER_NoInit;
    }

    /* 写入数据 */
    memcpy(buffer->p_tail, data, sizeof(data));
    buffer->p_tail += sizeof(data);

    /* 如果写入指针超出缓冲区，则将写入指针重置为缓冲区开头 */
    if (buffer->p_tail >= buffer->data + BUFFER_SIZE)
    {
        buffer->p_tail = buffer->data;
    }

    return BUFFER_OK;
}


Buffer_State_t RingBuffer_Read(Buffer_t *buffer, uint8_t *data)
{
    /* 判断缓冲区是否初始化 */
    if (buffer->p_head == NULL || buffer->p_tail == NULL)
    {
#if __BUFFER_DEBUG__ == 1
        printf("RingBuffer_Read: buffer->p_head = %p, buffer->p_tail = %p\n", buffer->p_head, buffer->p_tail);
        printf("RingBuffer_ReadError\n");
#endif
        return BUFFER_NoInit;
    }

    /* 读取数据 */
    memcpy(data, buffer->p_head, sizeof(data));
    buffer->p_head += sizeof(data);

    /* 如果读取指针超出缓冲区，则将读取指针重置为缓冲区开头 */
    if (buffer->p_head >= buffer->data + BUFFER_SIZE)
    {
        buffer->p_head = buffer->data;
    }

#if __BUFFER_DEBUG__ == 1
    printf("RingBuffer_Read: buffer->p_head = %p, buffer->p_tail = %p\n", buffer->p_head, buffer->p_tail);
    printf("RingBuffer_Read: data = %d\n", data);
#endif

    return BUFFER_OK;   
}