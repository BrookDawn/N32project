/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    oled_u8g2_adapter.h
  * @brief   U8g2库适配层头文件，将现有OLED驱动适配到u8g2库
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __OLED_U8G2_ADAPTER_H__
#define __OLED_U8G2_ADAPTER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "u8g2.h"
#include "oled_driver.h"
#include <stdint.h>
#include <stdbool.h>

/* Exported types ------------------------------------------------------------*/

/**
  * @brief  U8g2适配层句柄结构体
  */
typedef struct {
    u8g2_t u8g2;                     /* u8g2结构体 */
    I2C_HandleTypeDef *hi2c;         /* I2C句柄 */
    uint8_t i2c_addr;                /* I2C地址 */
    OLED_ChipType_t chip_type;       /* 芯片类型 */
    bool is_initialized;              /* 是否已初始化 */
} U8G2_OLED_HandleTypeDef;

/* Exported constants --------------------------------------------------------*/

/* I2C地址 */
#define U8G2_OLED_I2C_ADDR_0x78   0x78
#define U8G2_OLED_I2C_ADDR_0x7A   0x7A

/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief  初始化U8g2 OLED驱动（SSD1306 128x64 I2C）
  * @param  hu8g2: U8g2适配层句柄指针
  * @param  hi2c: I2C句柄指针
  * @param  i2c_addr: I2C地址
  * @retval 0=成功，其他=失败
  */
uint8_t U8G2_OLED_Init_SSD1306_128x64_I2C(U8G2_OLED_HandleTypeDef *hu8g2,
                                          I2C_HandleTypeDef *hi2c,
                                          uint8_t i2c_addr);

/**
  * @brief  初始化U8g2 OLED驱动（SH1106 128x64 I2C）
  * @param  hu8g2: U8g2适配层句柄指针
  * @param  hi2c: I2C句柄指针
  * @param  i2c_addr: I2C地址
  * @retval 0=成功，其他=失败
  */
uint8_t U8G2_OLED_Init_SH1106_128x64_I2C(U8G2_OLED_HandleTypeDef *hu8g2,
                                         I2C_HandleTypeDef *hi2c,
                                         uint8_t i2c_addr);

/**
  * @brief  获取u8g2结构体指针
  * @param  hu8g2: U8g2适配层句柄指针
  * @retval u8g2_t结构体指针
  */
u8g2_t* U8G2_OLED_GetU8g2(U8G2_OLED_HandleTypeDef *hu8g2);

/**
  * @brief  U8g2字节传输回调函数（内部使用）
  * @param  u8x8: u8x8结构体指针
  * @param  msg: 消息类型
  * @param  arg_int: 整数参数
  * @param  arg_ptr: 指针参数
  * @retval 1=成功，0=失败
  */
uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

/**
  * @brief  U8g2 GPIO和延时回调函数（内部使用）
  * @param  u8x8: u8x8结构体指针
  * @param  msg: 消息类型
  * @param  arg_int: 整数参数
  * @param  arg_ptr: 指针参数
  * @retval 1=成功，0=失败
  */
uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

#ifdef __cplusplus
}
#endif

#endif /* __OLED_U8G2_ADAPTER_H__ */
