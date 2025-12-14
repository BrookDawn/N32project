/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    oled_u8g2_example.h
  * @brief   U8g2 OLED使用示例头文件
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __OLED_U8G2_EXAMPLE_H__
#define __OLED_U8G2_EXAMPLE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "oled_u8g2_ui.h"

/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief  U8g2 OLED初始化和UI系统初始化
  */
void U8G2_OLED_Example_Init(void);

/**
  * @brief  左键按下处理（在按键中断或轮询中调用）
  * @note   左键功能：在当前菜单级别中向下遍历选项
  */
void U8G2_OLED_HandleLeftKeyPress(void);

/**
  * @brief  右键按下处理（在按键中断或轮询中调用）
  * @note   右键功能：确认并进入下一级菜单
  */
void U8G2_OLED_HandleRightKeyPress(void);

/**
  * @brief  更新主显示内容
  * @param  text: 主文本（如"CH"）
  * @param  number: 数字文本（如"4"）
  */
void U8G2_OLED_UpdateMainDisplay(const char *text, const char *number);

/**
  * @brief  更新信息区内容
  * @param  info: 信息文本
  */
void U8G2_OLED_UpdateInfo(const char *info);

/**
  * @brief  获取当前UI状态
  * @retval UI状态结构体指针
  */
UI_State_t* U8G2_OLED_GetUIState(void);

#ifdef __cplusplus
}
#endif

#endif /* __OLED_U8G2_EXAMPLE_H__ */
