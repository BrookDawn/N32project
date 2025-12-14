/**
  ******************************************************************************
  * @file    oled_direct_test.c
  * @brief   直接测试OLED显示（绕过U8G2 CAD层）
  *          用于对比验证I2C通信格式
  ******************************************************************************
  */

#include "oled_direct_test.h"
#include <string.h>

/* SSD1306 命令定义 */
#define SSD1306_DISPLAY_OFF          0xAE
#define SSD1306_DISPLAY_ON           0xAF
#define SSD1306_SET_CONTRAST         0x81
#define SSD1306_SET_DISPLAY_CLOCK    0xD5
#define SSD1306_SET_MULTIPLEX        0xA8
#define SSD1306_SET_DISPLAY_OFFSET   0xD3
#define SSD1306_SET_START_LINE       0x40
#define SSD1306_CHARGE_PUMP          0x8D
#define SSD1306_MEMORY_MODE          0x20
#define SSD1306_SEG_REMAP            0xA1
#define SSD1306_COM_SCAN_DEC         0xC8
#define SSD1306_SET_COM_PINS         0xDA
#define SSD1306_SET_PRECHARGE        0xD9
#define SSD1306_SET_VCOM_DETECT      0xDB
#define SSD1306_DISPLAY_ALL_OFF      0xA4
#define SSD1306_NORMAL_DISPLAY       0xA6
#define SSD1306_COLUMN_ADDR          0x21
#define SSD1306_PAGE_ADDR            0x22

/**
  * @brief  发送命令（直接方式，匹配简单测试）
  */
static HAL_StatusTypeDef SendCmd(I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t cmd)
{
    uint8_t buffer[2];
    buffer[0] = 0x00;  /* 控制字节：命令 */
    buffer[1] = cmd;
    return HAL_I2C_Master_Transmit(hi2c, addr, buffer, 2, 100);
}

/**
  * @brief  发送数据（直接方式）
  */
static HAL_StatusTypeDef SendData(I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t *data, uint16_t len)
{
    static uint8_t buffer[129];
    if(len > 128) len = 128;

    buffer[0] = 0x40;  /* 控制字节：数据 */
    memcpy(&buffer[1], data, len);

    return HAL_I2C_Master_Transmit(hi2c, addr, buffer, len + 1, 200);
}

/**
  * @brief  直接测试OLED（使用与simple_test相同的方式）
  */
HAL_StatusTypeDef OLED_DirectTest(I2C_HandleTypeDef *hi2c, uint8_t addr)
{
    HAL_StatusTypeDef status;
    uint8_t data[128];

    /* 延时确保OLED上电 */
    HAL_Delay(100);

    /* 初始化序列（与simple_test完全相同） */
    SendCmd(hi2c, addr, SSD1306_DISPLAY_OFF);
    SendCmd(hi2c, addr, SSD1306_SET_DISPLAY_CLOCK);
    SendCmd(hi2c, addr, 0x80);
    SendCmd(hi2c, addr, SSD1306_SET_MULTIPLEX);
    SendCmd(hi2c, addr, 0x3F);
    SendCmd(hi2c, addr, SSD1306_SET_DISPLAY_OFFSET);
    SendCmd(hi2c, addr, 0x00);
    SendCmd(hi2c, addr, SSD1306_SET_START_LINE | 0x00);
    SendCmd(hi2c, addr, SSD1306_CHARGE_PUMP);
    SendCmd(hi2c, addr, 0x14);
    SendCmd(hi2c, addr, SSD1306_MEMORY_MODE);
    SendCmd(hi2c, addr, 0x00);
    SendCmd(hi2c, addr, SSD1306_SEG_REMAP | 0x01);
    SendCmd(hi2c, addr, SSD1306_COM_SCAN_DEC);
    SendCmd(hi2c, addr, SSD1306_SET_COM_PINS);
    SendCmd(hi2c, addr, 0x12);
    SendCmd(hi2c, addr, SSD1306_SET_CONTRAST);
    SendCmd(hi2c, addr, 0xCF);
    SendCmd(hi2c, addr, SSD1306_SET_PRECHARGE);
    SendCmd(hi2c, addr, 0xF1);
    SendCmd(hi2c, addr, SSD1306_SET_VCOM_DETECT);
    SendCmd(hi2c, addr, 0x40);
    SendCmd(hi2c, addr, SSD1306_DISPLAY_ALL_OFF);
    SendCmd(hi2c, addr, SSD1306_NORMAL_DISPLAY);

    /* 重要：发送Display ON命令 */
    status = SendCmd(hi2c, addr, SSD1306_DISPLAY_ON);

    if(status != HAL_OK) return status;

    /* 等待显示器稳定 */
    HAL_Delay(50);

    /* 设置列地址范围 */
    SendCmd(hi2c, addr, SSD1306_COLUMN_ADDR);
    SendCmd(hi2c, addr, 0);
    SendCmd(hi2c, addr, 127);

    /* 设置页地址范围 */
    SendCmd(hi2c, addr, SSD1306_PAGE_ADDR);
    SendCmd(hi2c, addr, 0);
    SendCmd(hi2c, addr, 7);

    /* 绘制测试图案：水平条纹 */
    for(uint8_t page = 0; page < 8; page++)
    {
        for(uint8_t col = 0; col < 128; col++)
        {
            /* 交替条纹图案 */
            if(page % 2 == 0)
                data[col] = 0xFF;
            else
                data[col] = 0x00;
        }
        SendData(hi2c, addr, data, 128);
    }

    return HAL_OK;
}
