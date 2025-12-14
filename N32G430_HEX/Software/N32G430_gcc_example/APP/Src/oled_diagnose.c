/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    oled_diagnose.c
  * @brief   OLED诊断工具实现
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "oled_diagnose.h"
#include "hal_compat.h"
#include <string.h>
#include <stdio.h>

/* Private defines -----------------------------------------------------------*/
#define I2C_SCAN_TIMEOUT    10   /* I2C扫描超时（毫秒） */
#define I2C_TEST_TIMEOUT    100  /* I2C测试超时（毫秒） */

/* OLED常见I2C地址 */
#define OLED_ADDR_0X3C      0x3C  /* 7位地址 */
#define OLED_ADDR_0X3D      0x3D  /* 7位地址 */
#define OLED_ADDR_0X78      0x78  /* 8位写地址 */
#define OLED_ADDR_0X7A      0x7A  /* 8位写地址 */

/* SSD1306命令 */
#define SSD1306_CMD_DISPLAY_OFF     0xAE
#define SSD1306_CMD_DISPLAY_ON      0xAF
#define SSD1306_CMD_SET_CONTRAST    0x81

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  扫描I2C总线上的设备
  */
uint8_t OLED_I2C_Scan(I2C_HandleTypeDef *hi2c, uint8_t *found_addresses, uint8_t max_count)
{
    uint8_t found_count = 0;
    uint8_t test_data = 0;

    if(hi2c == NULL || found_addresses == NULL || max_count == 0)
    {
        return 0;
    }

    /* 扫描7位地址空间: 0x03 ~ 0x77 */
    for(uint16_t addr = 0x03; addr <= 0x77; addr++)
    {
        /* 尝试发送1字节数据，如果ACK则设备存在 */
        uint8_t write_addr = addr << 1;  /* 转换为8位写地址 */

        HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(hi2c, write_addr, &test_data, 0, I2C_SCAN_TIMEOUT);

        if(status == HAL_OK)
        {
            if(found_count < max_count)
            {
                found_addresses[found_count] = (uint8_t)addr;
                found_count++;
            }
        }

        HAL_Delay(1);  /* 短暂延时，避免总线冲突 */
    }

    return found_count;
}

/**
  * @brief  测试OLED基本通信
  */
uint8_t OLED_TestBasicComm(I2C_HandleTypeDef *hi2c, uint8_t i2c_addr)
{
    uint8_t cmd_buffer[2];
    HAL_StatusTypeDef status;

    if(hi2c == NULL)
    {
        return 1;
    }

    /* 测试1: 发送显示关闭命令 */
    cmd_buffer[0] = 0x00;  /* 命令模式 */
    cmd_buffer[1] = SSD1306_CMD_DISPLAY_OFF;
    status = HAL_I2C_Master_Transmit(hi2c, i2c_addr, cmd_buffer, 2, I2C_TEST_TIMEOUT);
    if(status != HAL_OK)
    {
        return 2;
    }

    HAL_Delay(10);

    /* 测试2: 发送对比度设置命令 */
    cmd_buffer[0] = 0x00;  /* 命令模式 */
    cmd_buffer[1] = SSD1306_CMD_SET_CONTRAST;
    status = HAL_I2C_Master_Transmit(hi2c, i2c_addr, cmd_buffer, 2, I2C_TEST_TIMEOUT);
    if(status != HAL_OK)
    {
        return 3;
    }

    HAL_Delay(10);

    /* 测试3: 发送对比度值 */
    cmd_buffer[0] = 0x00;  /* 命令模式 */
    cmd_buffer[1] = 0xCF;   /* 对比度值 */
    status = HAL_I2C_Master_Transmit(hi2c, i2c_addr, cmd_buffer, 2, I2C_TEST_TIMEOUT);
    if(status != HAL_OK)
    {
        return 4;
    }

    HAL_Delay(10);

    /* 测试4: 发送显示开启命令 */
    cmd_buffer[0] = 0x00;  /* 命令模式 */
    cmd_buffer[1] = SSD1306_CMD_DISPLAY_ON;
    status = HAL_I2C_Master_Transmit(hi2c, i2c_addr, cmd_buffer, 2, I2C_TEST_TIMEOUT);
    if(status != HAL_OK)
    {
        return 5;
    }

    return 0;  /* 所有测试通过 */
}

/**
  * @brief  诊断OLED连接和通信
  */
uint8_t OLED_Diagnose(I2C_HandleTypeDef *hi2c, OLED_DiagnoseResult_t *result)
{
    uint8_t found_addresses[16];
    uint8_t found_count;

    if(hi2c == NULL || result == NULL)
    {
        return 1;
    }

    /* 初始化结果 */
    memset(result, 0, sizeof(OLED_DiagnoseResult_t));

    /* 检查I2C是否初始化 */
    if(hi2c->Instance == NULL || hi2c->State == HAL_I2C_STATE_RESET)
    {
        snprintf(result->error_msg, sizeof(result->error_msg), "I2C not initialized");
        return 2;
    }
    result->i2c_initialized = true;

    /* 扫描I2C总线 */
    found_count = OLED_I2C_Scan(hi2c, found_addresses, sizeof(found_addresses));

    if(found_count == 0)
    {
        snprintf(result->error_msg, sizeof(result->error_msg), "No I2C devices found");
        return 3;
    }

    /* 查找OLED设备（常见地址: 0x3C, 0x3D） */
    for(uint8_t i = 0; i < found_count; i++)
    {
        if(found_addresses[i] == OLED_ADDR_0X3C || found_addresses[i] == OLED_ADDR_0X3D)
        {
            result->device_detected = true;
            result->detected_address = found_addresses[i];
            break;
        }
    }

    if(!result->device_detected)
    {
        /* 设备未找到，但总线上有其他设备 */
        snprintf(result->error_msg, sizeof(result->error_msg),
                 "OLED not found, but %d device(s) detected", found_count);
        return 4;
    }

    /* 测试基本通信 */
    uint8_t write_addr = result->detected_address << 1;  /* 转换为8位写地址 */
    uint8_t test_result = OLED_TestBasicComm(hi2c, write_addr);

    if(test_result == 0)
    {
        result->can_write_command = true;
        result->can_write_data = true;
        snprintf(result->error_msg, sizeof(result->error_msg), "OLED OK at 0x%02X", result->detected_address);
        return 0;
    }
    else
    {
        result->can_write_command = false;
        snprintf(result->error_msg, sizeof(result->error_msg),
                 "OLED found but comm test failed (step %d)", test_result);
        return 5;
    }
}
