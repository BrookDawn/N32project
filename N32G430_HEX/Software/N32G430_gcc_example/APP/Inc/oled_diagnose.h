/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    oled_diagnose.h
  * @brief   OLED诊断工具头文件
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __OLED_DIAGNOSE_H__
#define __OLED_DIAGNOSE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"

/* Exported types ------------------------------------------------------------*/

typedef struct {
    bool i2c_initialized;
    bool device_detected;
    uint8_t detected_address;
    bool can_write_command;
    bool can_write_data;
    char error_msg[64];
} OLED_DiagnoseResult_t;

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  诊断OLED连接和通信
  * @param  hi2c: I2C句柄指针
  * @param  result: 诊断结果结构体指针
  * @retval 0=成功，其他=失败
  */
uint8_t OLED_Diagnose(I2C_HandleTypeDef *hi2c, OLED_DiagnoseResult_t *result);

/**
  * @brief  扫描I2C总线上的设备
  * @param  hi2c: I2C句柄指针
  * @param  found_addresses: 找到的地址数组（最多16个）
  * @param  max_count: 数组最大容量
  * @retval 找到的设备数量
  */
uint8_t OLED_I2C_Scan(I2C_HandleTypeDef *hi2c, uint8_t *found_addresses, uint8_t max_count);

/**
  * @brief  测试OLED基本通信（发送初始化命令）
  * @param  hi2c: I2C句柄指针
  * @param  i2c_addr: I2C地址
  * @retval 0=成功，其他=失败
  */
uint8_t OLED_TestBasicComm(I2C_HandleTypeDef *hi2c, uint8_t i2c_addr);

#ifdef __cplusplus
}
#endif

#endif /* __OLED_DIAGNOSE_H__ */
