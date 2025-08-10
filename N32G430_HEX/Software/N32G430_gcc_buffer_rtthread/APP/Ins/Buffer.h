#ifndef __BUFFER_H__
#define __BUFFER_H__


#include <stdint.h>

/*
*  帧数据包的格式
*  | 帧头 | 帧长度 | 帧数据 | 校验和 | 帧尾 |
*  帧头：  固定值0x55    -> 帧头
*  帧长度：帧数据的长度   -> 有效的数据位数
*  帧数据：帧数据        -> 有效数据
*  校验和：帧数据的校验和 -> 校验和 0x00
*  帧尾：  固定值0xAA    -> 帧尾    
*  校验和： 帧头+帧长度+帧数据+帧尾的最后两位
*/


#define __BUFFER_DEBUG__  1

#define BUFFER_SIZE 40
// 移除重复的uint8_t定义，因为stdint.h已经定义了
// #define uint8_t  unsigned char
// #define uint16_t unsigned int


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
    uint16_t *p_read;              //读指针
    uint16_t  data_length;         //数据长度
} Buffer_t;



typedef enum
{
    BUFFER_OK              = 0,
    BUFFER_InitError       = -1,
    BUFFER_ReadError       = -2,
} Buffer_State_t;


void RingBuffer_Init(Buffer_t *buffer);                                                 //初始化缓冲区

Buffer_State_t RingBuffer_WriteArray(Buffer_t *buffer, uint8_t *data, uint16_t length); //写入数组
Buffer_State_t RingBuffer_ReadArray(Buffer_t *buffer, uint8_t *data, uint16_t length);  //读取数组
Buffer_State_t Data_Check(Buffer_t *buffer);                                            //数据校验
uint8_t Data_Read(Buffer_t *buffer, uint8_t *data);                                     //数据读取


uint16_t Data_Pack(Buffer_t *buffer);                                                   //数据打包


#endif // __BUFFER_H__