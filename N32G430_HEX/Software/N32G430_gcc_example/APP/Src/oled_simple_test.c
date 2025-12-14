/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    oled_simple_test.c
  * @brief   简单的OLED测试 - 使用原始驱动直接点亮
  ******************************************************************************
  */
/* USER CODE END Header */

#include "oled_simple_test.h"
#include "i2c.h"
#include "hal_compat.h"
#include <string.h>

/* OLED I2C地址 */
#define OLED_I2C_ADDR    0x78

/* 控制字节 */
#define OLED_CMD_MODE    0x00
#define OLED_DATA_MODE   0x40

/* SSD1306命令 */
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
#define SSD1306_DISPLAY_ALL_ON       0xA5
#define SSD1306_DISPLAY_ALL_OFF      0xA4
#define SSD1306_NORMAL_DISPLAY       0xA6
#define SSD1306_INVERT_DISPLAY       0xA7
#define SSD1306_COLUMN_ADDR          0x21
#define SSD1306_PAGE_ADDR            0x22

/* 私有函数 */
static HAL_StatusTypeDef OLED_SendCmd(uint8_t cmd)
{
    uint8_t buffer[2];
    buffer[0] = OLED_CMD_MODE;
    buffer[1] = cmd;
    return HAL_I2C_Master_Transmit(&hi2c1, OLED_I2C_ADDR, buffer, 2, 100);
}

static HAL_StatusTypeDef OLED_SendData(uint8_t *data, uint16_t len)
{
    static uint8_t buffer[129];

    if(len > 128) len = 128;

    buffer[0] = OLED_DATA_MODE;
    memcpy(&buffer[1], data, len);

    return HAL_I2C_Master_Transmit(&hi2c1, OLED_I2C_ADDR, buffer, len + 1, 200);
}

/**
  * @brief  初始化SSD1306 OLED
  */
HAL_StatusTypeDef OLED_SimpleTest_Init(void)
{
    HAL_StatusTypeDef status;

    /* 短暂延时确保OLED上电稳定 */
    HAL_Delay(100);

    /* 关闭显示 */
    status = OLED_SendCmd(SSD1306_DISPLAY_OFF);
    if(status != HAL_OK) return status;

    /* 设置显示时钟分频 */
    OLED_SendCmd(SSD1306_SET_DISPLAY_CLOCK);
    OLED_SendCmd(0x80);

    /* 设置多路复用比 */
    OLED_SendCmd(SSD1306_SET_MULTIPLEX);
    OLED_SendCmd(0x3F);  /* 64行 */

    /* 设置显示偏移 */
    OLED_SendCmd(SSD1306_SET_DISPLAY_OFFSET);
    OLED_SendCmd(0x00);

    /* 设置起始行 */
    OLED_SendCmd(SSD1306_SET_START_LINE | 0x00);

    /* 使能电荷泵 */
    OLED_SendCmd(SSD1306_CHARGE_PUMP);
    OLED_SendCmd(0x14);

    /* 设置内存寻址模式（水平） */
    OLED_SendCmd(SSD1306_MEMORY_MODE);
    OLED_SendCmd(0x00);

    /* 段重映射 */
    OLED_SendCmd(SSD1306_SEG_REMAP | 0x01);

    /* COM扫描方向 */
    OLED_SendCmd(SSD1306_COM_SCAN_DEC);

    /* 设置COM引脚配置 */
    OLED_SendCmd(SSD1306_SET_COM_PINS);
    OLED_SendCmd(0x12);

    /* 设置对比度 */
    OLED_SendCmd(SSD1306_SET_CONTRAST);
    OLED_SendCmd(0xCF);

    /* 设置预充电周期 */
    OLED_SendCmd(SSD1306_SET_PRECHARGE);
    OLED_SendCmd(0xF1);

    /* 设置VCOM检测电平 */
    OLED_SendCmd(SSD1306_SET_VCOM_DETECT);
    OLED_SendCmd(0x40);

    /* 恢复RAM显示 */
    OLED_SendCmd(SSD1306_DISPLAY_ALL_OFF);

    /* 正常显示 */
    OLED_SendCmd(SSD1306_NORMAL_DISPLAY);

    /* 开启显示 */
    status = OLED_SendCmd(SSD1306_DISPLAY_ON);

    return status;
}

/**
  * @brief  清屏（全白）
  */
void OLED_SimpleTest_Clear(void)
{
    uint8_t data[128];

    /* 设置列地址范围 */
    OLED_SendCmd(SSD1306_COLUMN_ADDR);
    OLED_SendCmd(0);    /* 起始列 */
    OLED_SendCmd(127);  /* 结束列 */

    /* 设置页地址范围 */
    OLED_SendCmd(SSD1306_PAGE_ADDR);
    OLED_SendCmd(0);    /* 起始页 */
    OLED_SendCmd(7);    /* 结束页 */

    /* 填充0x00（全黑） */
    memset(data, 0x00, 128);

    for(uint8_t page = 0; page < 8; page++)
    {
        OLED_SendData(data, 128);
    }
}

/**
  * @brief  测试显示（显示棋盘格）
  */
void OLED_SimpleTest_ShowPattern(void)
{
    uint8_t data[128];

    /* 设置列地址范围 */
    OLED_SendCmd(SSD1306_COLUMN_ADDR);
    OLED_SendCmd(0);
    OLED_SendCmd(127);

    /* 设置页地址范围 */
    OLED_SendCmd(SSD1306_PAGE_ADDR);
    OLED_SendCmd(0);
    OLED_SendCmd(7);

    /* 显示不同的图案在每一页 */
    for(uint8_t page = 0; page < 8; page++)
    {
        for(uint8_t col = 0; col < 128; col++)
        {
            /* 棋盘格图案 */
            if((col / 8 + page) % 2 == 0)
            {
                data[col] = 0xFF;
            }
            else
            {
                data[col] = 0x00;
            }
        }
        OLED_SendData(data, 128);
    }
}

/**
  * @brief  全屏填充
  */
void OLED_SimpleTest_FillScreen(uint8_t pattern)
{
    uint8_t data[128];

    /* 设置列地址范围 */
    OLED_SendCmd(SSD1306_COLUMN_ADDR);
    OLED_SendCmd(0);
    OLED_SendCmd(127);

    /* 设置页地址范围 */
    OLED_SendCmd(SSD1306_PAGE_ADDR);
    OLED_SendCmd(0);
    OLED_SendCmd(7);

    /* 填充指定图案 */
    memset(data, pattern, 128);

    for(uint8_t page = 0; page < 8; page++)
    {
        OLED_SendData(data, 128);
    }
}
