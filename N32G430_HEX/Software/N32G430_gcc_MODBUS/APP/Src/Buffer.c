#include "Buffer.h"
#include <stdio.h>
#include <string.h>


/**
 * @brief        初始化环形缓冲区 
 * @param buffer 缓冲区结构体
 * @note         头指针和尾指针都为0，清空缓冲区数据
 */
void RingBuffer_Init(Buffer_t *buffer)
{
    buffer->p_head = (uint16_t *)buffer->data;
    buffer->p_tail = (uint16_t *)buffer->data; 
    buffer->p_read = (uint16_t *)buffer->data;
    buffer->data_length = 0;
    memset(buffer->data, 0, sizeof(buffer->data));

#if __BUFFER_DEBUG__ == 1
    printf("RingBuffer_Init: buffer->p_head = %p, buffer->p_tail = %p\n", buffer->p_head, buffer->p_tail);
#endif
}

/**
 * @brief        使用memcpy写入数组
 * @param buffer 缓冲区结构体
 * @param data   数据数组
 * @param length 数组长度
 * @return 状态
 */
Buffer_State_t RingBuffer_WriteArray(Buffer_t *buffer, uint8_t *data, uint16_t length)
{
    /* 判断缓冲区是否初始化 */
    if (buffer->p_head == NULL || buffer->p_tail == NULL)
    {
#if __BUFFER_DEBUG__ == 1
        printf("RingBuffer_WriteArray: InitError\n");
#endif
        return BUFFER_InitError;
    }

    /* 判断参数有效性 */
    if (data == NULL || length == 0)
    {
#if __BUFFER_DEBUG__ == 1
        printf("RingBuffer_WriteArray: Invalid parameters\n");
#endif
        return BUFFER_InitError;
    }

    uint16_t remaining = length;
    uint16_t offset = 0;
    
    while (remaining > 0)
    {
        /* 计算当前可连续写入的字节数 */
        uint16_t available_to_end = BUFFER_SIZE - ((uint8_t *)buffer->p_tail - buffer->data);
        uint16_t write_size = (remaining < available_to_end) ? remaining : available_to_end;
        
        /* 检查是否需要移动头指针（缓冲区满的情况） */
        uint16_t *next_tail = (uint16_t *)((uint8_t *)buffer->p_tail + write_size);
        if ((uint8_t *)next_tail >= buffer->data + BUFFER_SIZE)
        {
            next_tail = (uint16_t *)(buffer->data + ((uint8_t *)next_tail - (buffer->data + BUFFER_SIZE)));
        }
        
        if (next_tail == buffer->p_head)
        {
            /* 缓冲区满，移动头指针 */
            uint16_t head_move = write_size;
            buffer->p_head = (uint16_t *)((uint8_t *)buffer->p_head + head_move);
            if ((uint8_t *)buffer->p_head >= buffer->data + BUFFER_SIZE)
            {
                buffer->p_head = (uint16_t *)(buffer->data + ((uint8_t *)buffer->p_head - (buffer->data + BUFFER_SIZE)));
            }
#if __BUFFER_DEBUG__ == 1
            printf("RingBuffer_WriteArray: Buffer full, moving head by %d bytes\n", head_move);
#endif
        }
        
        /* 使用memcpy批量复制 */
        memcpy((uint8_t *)buffer->p_tail, data + offset, write_size);
        
        /* 更新尾指针 */
        buffer->p_tail = (uint16_t *)((uint8_t *)buffer->p_tail + write_size);
        if ((uint8_t *)buffer->p_tail >= buffer->data + BUFFER_SIZE)
        {
            buffer->p_tail = (uint16_t *)buffer->data;
        }
        
#if __BUFFER_DEBUG__ == 1
        printf("RingBuffer_WriteArray: Copied %d bytes (offset %d), p_head = %p, p_tail = %p\n", 
               write_size, offset, buffer->p_head, buffer->p_tail);
#endif
        
        remaining -= write_size;
        offset += write_size;
    }

    return BUFFER_OK;
}



Buffer_State_t RingBuffer_ReadArray(Buffer_t *buffer, uint8_t *data, uint16_t length)
{
    if (buffer->p_head == NULL || buffer->p_tail == NULL)
    {
#if __BUFFER_DEBUG__ == 1   
        printf("RingBuffer_ReadArray: buffer->p_head = %p, buffer->p_tail = %p\n", buffer->p_head, buffer->p_tail);
        printf("RingBuffer_ReadArray: InitError\n");
#endif
        return BUFFER_InitError;
    }

    if (data == NULL || length == 0)
    {
#if __BUFFER_DEBUG__ == 1
        printf("RingBuffer_ReadArray: Invalid parameters\n");
#endif
        return BUFFER_InitError;
    }

    for (uint16_t i = 0; i < length; i++)
    {
        data[i] = *((uint8_t *)buffer->p_head);
        buffer->p_head = (uint16_t *)((uint8_t *)buffer->p_head + sizeof(uint8_t));
        if ((uint8_t *)buffer->p_head >= buffer->data + BUFFER_SIZE)
        {
            buffer->p_head = (uint16_t *)buffer->data;
        }

#if __BUFFER_DEBUG__ == 1   
printf("RingBuffer_ReadArray: Read data[%d] = %d, p_head = %p, p_tail = %p\n",  
        i, data[i], buffer->p_head, buffer->p_tail);
#endif
    }

        return BUFFER_OK;
}


 /**
  * @brief        数据读取
  * @param buffer 缓冲区结构体
  * @param data   输出数据
  * @return       有效数据长度（0表示失败）
  * @note         读取有效的数据帧，返回实际读取的数据长度
  */
 uint8_t Data_Read(Buffer_t *buffer, uint8_t *data)
 {
     if (buffer->p_read == NULL || data == NULL)
     {
#if __BUFFER_DEBUG__ == 1
         printf("Data_Read: Invalid parameters\n");
#endif
         return 0;
     }

     // 先检查是否有有效帧
     if (Data_Check(buffer) != BUFFER_OK)
     {
#if __BUFFER_DEBUG__ == 1
         printf("Data_Read: No valid frame found\n");
#endif
         return 0;
     }

     // 读取帧长度
     uint8_t frame_length = *((uint8_t *)buffer->p_read + 1);
     
     // 读取帧数据
     uint16_t *p_temp = (uint16_t *)((uint8_t *)buffer->p_read + 2); // 跳过帧头和帧长度
     
     for (uint8_t i = 0; i < frame_length; i++)
     {
         data[i] = *((uint8_t *)p_temp);
         p_temp = (uint16_t *)((uint8_t *)p_temp + 1);
         if ((uint8_t *)p_temp >= buffer->data + BUFFER_SIZE)
         {
             p_temp = (uint16_t *)buffer->data;
         }
     }

     // 移动读指针到下一帧开始位置
     buffer->p_read = (uint16_t *)((uint8_t *)buffer->p_read + frame_length + 4); // 跳过帧头、帧长度、帧数据、校验和、帧尾
     if ((uint8_t *)buffer->p_read >= buffer->data + BUFFER_SIZE)
     {
         buffer->p_read = (uint16_t *)(buffer->data + ((uint8_t *)buffer->p_read - (buffer->data + BUFFER_SIZE)));
     }

#if __BUFFER_DEBUG__ == 1
     printf("Data_Read: Read frame data, length = %d\n", frame_length);
#endif

     return frame_length;
 }


/**
 * @brief        数据打包
 * @param buffer 缓冲区结构体
 * @return       状态
 * @note         打包帧头、帧长度、帧数据、帧尾、校验和
 */
uint16_t Data_Pack(Buffer_t *buffer)
{
    // 这里可以实现数据打包功能
    // 暂时返回0，表示未实现
    return 0;
}

/**
 * @brief        数据校验
 * @param buffer 缓冲区结构体
 * @return       状态
 * @note         校验帧头、帧长度、帧数据、帧尾、校验和
 */
Buffer_State_t Data_Check(Buffer_t *buffer)
{
    //判断读指针是否为空
    if (buffer->p_read == NULL)
    {
        return BUFFER_InitError;
    }

    uint16_t data_length = 0;
    uint8_t checksum = 0;
    uint8_t temp_checksum = 0;
    uint16_t *p_temp = buffer->p_read;
    uint16_t *start_position = buffer->p_read; // 记录起始位置，防止无限循环
    uint16_t search_count = 0; // 搜索计数器

    while(search_count < BUFFER_SIZE)/* 循环检测数据，最多搜索整个缓冲区 */
    {
        search_count++;
        
        /* 判断帧头 */
        if (*((uint8_t *)buffer->p_read) != 0x55)
        {
            buffer->p_read = (uint16_t *)((uint8_t *)buffer->p_read + 1);
            if ((uint8_t *)buffer->p_read >= buffer->data + BUFFER_SIZE)
            {
                buffer->p_read = (uint16_t *)buffer->data;
            }
            continue;
        }

#if __BUFFER_DEBUG__ == 1
        printf("Data_Check: Found frame header at position %d\n", 
               (int)((uint8_t *)buffer->p_read - buffer->data));
#endif

        /* 检查是否有足够数据读取帧长度 */
        uint8_t *next_byte = (uint8_t *)buffer->p_read + 1;
        if (next_byte >= buffer->data + BUFFER_SIZE)
        {
            next_byte = buffer->data;
        }

        /* 读取帧长度 */
        data_length = *next_byte;
        
#if __BUFFER_DEBUG__ == 1
        printf("Data_Check: Frame length = %d\n", data_length);
#endif

        /* 判断帧长度是否合理 */
        if (data_length == 0 || data_length > BUFFER_SIZE - 4)
        {
            buffer->p_read = (uint16_t *)((uint8_t *)buffer->p_read + 1);
            if ((uint8_t *)buffer->p_read >= buffer->data + BUFFER_SIZE)
            {
                buffer->p_read = (uint16_t *)buffer->data;
            }
            continue;
        }

        /* 计算校验和：帧头 + 帧长度 + 帧数据 + 帧尾 */
        temp_checksum = 0;
        p_temp = buffer->p_read;
        
        // 帧头
        temp_checksum += *((uint8_t *)p_temp);
        p_temp = (uint16_t *)((uint8_t *)p_temp + 1);
        if ((uint8_t *)p_temp >= buffer->data + BUFFER_SIZE)
        {
            p_temp = (uint16_t *)buffer->data;
        }
        
        // 帧长度
        temp_checksum += *((uint8_t *)p_temp);
        p_temp = (uint16_t *)((uint8_t *)p_temp + 1);
        if ((uint8_t *)p_temp >= buffer->data + BUFFER_SIZE)
        {
            p_temp = (uint16_t *)buffer->data;
        }
        
        // 帧数据
        for (uint16_t i = 0; i < data_length; i++)
        {
            temp_checksum += *((uint8_t *)p_temp);
            p_temp = (uint16_t *)((uint8_t *)p_temp + 1);
            if ((uint8_t *)p_temp >= buffer->data + BUFFER_SIZE)
            {
                p_temp = (uint16_t *)buffer->data;
            }
        }
        
        /* 读取存储的校验和 */
        checksum = *((uint8_t *)p_temp);
        p_temp = (uint16_t *)((uint8_t *)p_temp + 1);
        if ((uint8_t *)p_temp >= buffer->data + BUFFER_SIZE)
        {
            p_temp = (uint16_t *)buffer->data;
        }
        
        /* 读取帧尾并加入校验和计算 */
        uint8_t frame_tail = *((uint8_t *)p_temp);
        temp_checksum += frame_tail;

#if __BUFFER_DEBUG__ == 1
        printf("Data_Check: Calculated checksum = 0x%02X, Stored checksum = 0x%02X\n", 
               temp_checksum & 0xFF, checksum);
        printf("Data_Check: Frame tail = 0x%02X\n", frame_tail);
#endif

        /* 检测校验和 */
        if ((temp_checksum & 0xFF) != checksum)
        {
            // 校验和错误，移动到下一个字节继续查找
            buffer->p_read = (uint16_t *)((uint8_t *)buffer->p_read + 1);
            if ((uint8_t *)buffer->p_read >= buffer->data + BUFFER_SIZE)
            {
                buffer->p_read = (uint16_t *)buffer->data;
            }
            continue;
        }

        /* 判断帧尾 */
        if (frame_tail != 0xAA)
        {
            // 帧尾错误，移动到下一个字节继续查找
            buffer->p_read = (uint16_t *)((uint8_t *)buffer->p_read + 1);
            if ((uint8_t *)buffer->p_read >= buffer->data + BUFFER_SIZE)
            {
                buffer->p_read = (uint16_t *)buffer->data;
            }
            continue;
        }

        /* 所有检查都通过，找到有效帧 */
#if __BUFFER_DEBUG__ == 1
        printf("Data_Check: Valid frame found, length = %d\n", data_length);
#endif
        return BUFFER_OK;
    }
    
    // 搜索完整个缓冲区都没有找到有效帧
    buffer->p_read = start_position; // 恢复原始位置
    return BUFFER_ReadError;
}