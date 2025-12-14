/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    oled_u8g2_example.c
  * @brief   U8g2 OLED使用示例 - 展示UI系统和菜单导航
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
#include "oled_u8g2_example.h"
#include "oled_u8g2_adapter.h"
#include "oled_u8g2_ui.h"
#include "i2c.h"

/* Private variables ---------------------------------------------------------*/
static U8G2_OLED_HandleTypeDef g_u8g2_oled;
static UI_State_t g_ui_state;

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  U8g2 OLED初始化和UI系统初始化
  * @note   根据实际使用的芯片类型选择初始化函数
  */
void U8G2_OLED_Example_Init(void)
{
    /* 初始化OLED硬件驱动 */
    /* 方式1: 使用SSD1306 128x64 I2C */
    U8G2_OLED_Init_SSD1306_128x64_I2C(&g_u8g2_oled, &hi2c1, 0x78);

    /* 方式2: 使用SH1106 128x64 I2C（根据实际芯片选择）*/
    // U8G2_OLED_Init_SH1106_128x64_I2C(&g_u8g2_oled, &hi2c1, 0x78);

    /* 初始化UI状态 */
    UI_Init(&g_ui_state);

    /* 绘制初始界面 */
    u8g2_t *u8g2 = U8G2_OLED_GetU8g2(&g_u8g2_oled);
    if(u8g2 != NULL)
    {
        UI_Draw(u8g2, &g_ui_state);
    }
}

/**
  * @brief  左键按下处理（在按键中断或轮询中调用）
  * @note   左键功能：在当前菜单级别中向下遍历选项
  */
void U8G2_OLED_HandleLeftKeyPress(void)
{
    /* 处理左键逻辑 */
    UI_HandleLeftKey(&g_ui_state);

    /* 更新显示 */
    u8g2_t *u8g2 = U8G2_OLED_GetU8g2(&g_u8g2_oled);
    if(u8g2 != NULL)
    {
        UI_Draw(u8g2, &g_ui_state);
    }
}

/**
  * @brief  右键按下处理（在按键中断或轮询中调用）
  * @note   右键功能：确认并进入下一级菜单
  */
void U8G2_OLED_HandleRightKeyPress(void)
{
    /* 处理右键逻辑 */
    UI_HandleRightKey(&g_ui_state);

    /* 更新显示 */
    u8g2_t *u8g2 = U8G2_OLED_GetU8g2(&g_u8g2_oled);
    if(u8g2 != NULL)
    {
        UI_Draw(u8g2, &g_ui_state);
    }
}

/**
  * @brief  更新主显示内容示例
  * @note   可在应用程序中调用此函数更新显示内容
  */
void U8G2_OLED_UpdateMainDisplay(const char *text, const char *number)
{
    /* 更新主显示区内容 */
    UI_SetMainDisplay(&g_ui_state, text, number);

    /* 刷新显示 */
    u8g2_t *u8g2 = U8G2_OLED_GetU8g2(&g_u8g2_oled);
    if(u8g2 != NULL)
    {
        UI_Draw(u8g2, &g_ui_state);
    }
}

/**
  * @brief  更新信息区内容示例
  * @note   可用于显示状态、提示等信息
  */
void U8G2_OLED_UpdateInfo(const char *info)
{
    /* 更新信息区内容 */
    UI_SetInfo(&g_ui_state, info);

    /* 刷新显示 */
    u8g2_t *u8g2 = U8G2_OLED_GetU8g2(&g_u8g2_oled);
    if(u8g2 != NULL)
    {
        UI_Draw(u8g2, &g_ui_state);
    }
}

/**
  * @brief  获取当前UI状态（用于调试或高级功能）
  * @retval UI状态结构体指针
  */
UI_State_t* U8G2_OLED_GetUIState(void)
{
    return &g_ui_state;
}
