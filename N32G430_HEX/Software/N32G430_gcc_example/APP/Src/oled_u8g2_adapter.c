/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    oled_u8g2_adapter.c
  * @brief   U8g2库适配层实现文件
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "oled_u8g2_adapter.h"
#include <string.h>

/* Private defines -----------------------------------------------------------*/
#define I2C_TIMEOUT     100   /* I2C超时时间（毫秒） */

/* SSD1306 I2C控制字节 */
#define OLED_CONTROL_BYTE_CMD     0x00  /* 命令模式 */
#define OLED_CONTROL_BYTE_DATA    0x40  /* 数据模式 */

/* Private variables ---------------------------------------------------------*/
static I2C_HandleTypeDef *g_hi2c = NULL;
static uint8_t g_i2c_addr = 0;

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  U8g2字节传输回调函数（I2C通信）
  * @param  u8x8: u8x8结构体指针
  * @param  msg: 消息类型
  * @param  arg_int: 整数参数
  * @param  arg_ptr: 指针参数
  * @retval 1=成功，0=失败
  *
  * @note   重要：U8G2的CAD层会在发送命令/数据前添加控制字节(0x00/0x40)
  *         但是u8x8_ClearDisplay/FillDisplay会绕过CAD层直接调用byte层
  *         因此需要在END_TRANSFER时检测并自动添加控制字节
  */
uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    static uint8_t buffer[130];  /* 预留空间给控制字节 */
    static uint8_t buf_idx;
    uint8_t *data;

    switch(msg)
    {
        case U8X8_MSG_BYTE_SEND:  /* 等同于 U8X8_MSG_CAD_SEND_DATA */
            /* 收集字节到buffer */
            data = (uint8_t *)arg_ptr;
            while(arg_int > 0)
            {
                buffer[buf_idx++] = *data;
                data++;
                arg_int--;
            }
            break;

        case U8X8_MSG_BYTE_INIT:
            /* I2C已经初始化，不需要额外操作 */
            break;

        case U8X8_MSG_BYTE_SET_DC:
            /* CAD层使用此消息设置命令/数据模式 */
            /* 对于I2C不需要处理，CAD层会自己发送控制字节 */
            break;

        case U8X8_MSG_BYTE_START_TRANSFER:
            buf_idx = 0;  /* 从索引0开始 */
            break;

        case U8X8_MSG_BYTE_END_TRANSFER:
            if(g_hi2c != NULL && buf_idx > 0)
            {
                uint8_t *send_buf = buffer;
                uint16_t send_len = buf_idx;

                /* 改进的检测逻辑：
                 * 规则1: 如果buf_idx >= 8，肯定是tile数据（每个tile是8字节），需要0x40
                 * 规则2: 如果buffer[0]不是0x00或0x40，也是数据
                 * 规则3: 否则，已经有CAD层添加的控制字节，直接发送
                 */
                if(buf_idx >= 8 || (buffer[0] != 0x00 && buffer[0] != 0x40))
                {
                    /* 需要添加数据控制字节0x40 */
                    memmove(&buffer[1], &buffer[0], buf_idx);
                    buffer[0] = 0x40;
                    send_len = buf_idx + 1;
                }

                /* 发送buffer内容 */
                HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(g_hi2c, g_i2c_addr, send_buf, send_len, I2C_TIMEOUT);
                if(status != HAL_OK)
                {
                    return 0;  /* 传输失败 */
                }
            }
            break;

        default:
            return 0;
    }

    return 1;
}

/**
  * @brief  U8g2 GPIO和延时回调函数
  * @param  u8x8: u8x8结构体指针
  * @param  msg: 消息类型
  * @param  arg_int: 整数参数
  * @param  arg_ptr: 指针参数
  * @retval 1=成功，0=失败
  */
uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch(msg)
    {
        case U8X8_MSG_GPIO_AND_DELAY_INIT:
            /* GPIO已经初始化，不需要额外操作 */
            break;

        case U8X8_MSG_DELAY_MILLI:
            /* 毫秒延时 */
            HAL_Delay(arg_int);
            break;

        case U8X8_MSG_DELAY_10MICRO:
            /* 10微秒延时 - 假设系统时钟108MHz，每个NOP约10ns */
            /* 10us = 1000个NOP */
            for(uint32_t j = 0; j < arg_int; j++)
            {
                for(volatile uint32_t i = 0; i < 1000; i++)
                {
                    __NOP();
                }
            }
            break;

        case U8X8_MSG_DELAY_100NANO:
            /* 100纳秒延时 - 约10个NOP */
            for(uint32_t j = 0; j < arg_int; j++)
            {
                for(volatile uint32_t i = 0; i < 10; i++)
                {
                    __NOP();
                }
            }
            break;

        case U8X8_MSG_GPIO_I2C_CLOCK:
            /* I2C时钟引脚控制（硬件I2C不需要） */
            break;

        case U8X8_MSG_GPIO_I2C_DATA:
            /* I2C数据引脚控制（硬件I2C不需要） */
            break;

        default:
            return 0;
    }

    return 1;
}

/**
  * @brief  初始化U8g2 OLED驱动（SSD1306 128x64 I2C）
  * @param  hu8g2: U8g2适配层句柄指针
  * @param  hi2c: I2C句柄指针
  * @param  i2c_addr: I2C地址
  * @retval 0=成功，其他=失败
  */
uint8_t U8G2_OLED_Init_SSD1306_128x64_I2C(U8G2_OLED_HandleTypeDef *hu8g2,
                                          I2C_HandleTypeDef *hi2c,
                                          uint8_t i2c_addr)
{
    if(hu8g2 == NULL || hi2c == NULL)
    {
        return 1;
    }

    /* 保存I2C句柄和地址到全局变量 */
    g_hi2c = hi2c;
    g_i2c_addr = i2c_addr;

    /* 保存参数到句柄 */
    hu8g2->hi2c = hi2c;
    hu8g2->i2c_addr = i2c_addr;
    hu8g2->chip_type = OLED_TYPE_SSD1306;

    /* 使用u8g2全缓冲模式初始化SSD1306 */
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&hu8g2->u8g2,
                                           U8G2_R0,
                                           u8x8_byte_hw_i2c,
                                           u8x8_gpio_and_delay);

    /* 初始化u8g2 */
    u8g2_InitDisplay(&hu8g2->u8g2);

    /* 唤醒显示 - 发送Display ON命令 */
    u8g2_SetPowerSave(&hu8g2->u8g2, 0);

    /* 测试1：使用u8x8直接清空OLED */
    u8x8_ClearDisplay(u8g2_GetU8x8(&hu8g2->u8g2));
    HAL_Delay(500);

    /* 测试2：使用u8x8直接填充全白（验证数据发送） */
    u8x8_FillDisplay(u8g2_GetU8x8(&hu8g2->u8g2));
    HAL_Delay(500);

    /* 测试3：清空后绘制文字 */
    u8g2_ClearBuffer(&hu8g2->u8g2);
    u8g2_SetFont(&hu8g2->u8g2, u8g2_font_ncenB14_tr);
    u8g2_DrawStr(&hu8g2->u8g2, 0, 20, "HELLO!");
    u8g2_DrawFrame(&hu8g2->u8g2, 0, 25, 128, 10);
    u8g2_SendBuffer(&hu8g2->u8g2);

    hu8g2->is_initialized = true;

    return 0;
}

/**
  * @brief  初始化U8g2 OLED驱动（SH1106 128x64 I2C）
  * @param  hu8g2: U8g2适配层句柄指针
  * @param  hi2c: I2C句柄指针
  * @param  i2c_addr: I2C地址
  * @retval 0=成功，其他=失败
  */
uint8_t U8G2_OLED_Init_SH1106_128x64_I2C(U8G2_OLED_HandleTypeDef *hu8g2,
                                         I2C_HandleTypeDef *hi2c,
                                         uint8_t i2c_addr)
{
    if(hu8g2 == NULL || hi2c == NULL)
    {
        return 1;
    }

    /* 保存I2C句柄和地址到全局变量 */
    g_hi2c = hi2c;
    g_i2c_addr = i2c_addr;

    /* 保存参数到句柄 */
    hu8g2->hi2c = hi2c;
    hu8g2->i2c_addr = i2c_addr;
    hu8g2->chip_type = OLED_TYPE_SH1106;

    /* 使用u8g2全缓冲模式初始化SH1106 */
    u8g2_Setup_sh1106_i2c_128x64_noname_f(&hu8g2->u8g2,
                                          U8G2_R0,
                                          u8x8_byte_hw_i2c,
                                          u8x8_gpio_and_delay);

    /* 初始化u8g2 */
    u8g2_InitDisplay(&hu8g2->u8g2);
    u8g2_SetPowerSave(&hu8g2->u8g2, 0);  /* 唤醒显示 */
    u8g2_ClearBuffer(&hu8g2->u8g2);
    u8g2_SendBuffer(&hu8g2->u8g2);

    hu8g2->is_initialized = true;

    return 0;
}

/**
  * @brief  获取u8g2结构体指针
  * @param  hu8g2: U8g2适配层句柄指针
  * @retval u8g2_t结构体指针
  */
u8g2_t* U8G2_OLED_GetU8g2(U8G2_OLED_HandleTypeDef *hu8g2)
{
    if(hu8g2 == NULL || !hu8g2->is_initialized)
    {
        return NULL;
    }

    return &hu8g2->u8g2;
}
