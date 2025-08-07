#ifndef __BUFFER_H__
#define __BUFFER_H__


#include "main.h"

#define __BUFFER_DEBUG__  1

#define BUFFER_SIZE 1024
#define NULL 0

/**
 * @brief 缓冲区结构体
 * 
 * @param data 数据
 * @param head 头指针
 * @param tail 尾指针
 * @return Buffer_t 缓冲区结构体
 */
typedef struct
{
    uint8_t  data[BUFFER_SIZE];
    uint16_t *p_head;              //头地址指针
    uint16_t *p_tail;              //尾地址指针
} Buffer_t;



typedef enum
{
    BUFFER_OK = 0,
    BUFFER_NoInit = -1,
} Buffer_State_t;


void RingBuffer_Init(Buffer_t *buffer);                                      //初始化缓冲区
Buffer_State_t RingBuffer_Write(Buffer_t *buffer, uint8_t *data);            //写入数据
Buffer_State_t RingBuffer_Read(Buffer_t *buffer, uint8_t *data);             //读取数据



#endif // __BUFFER_H__