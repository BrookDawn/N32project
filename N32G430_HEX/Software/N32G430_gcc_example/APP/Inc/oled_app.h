/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    oled_app.h
  * @brief   OLED应用层头文件 - 页面管理器应用
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __OLED_APP_H__
#define __OLED_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief  初始化OLED应用
  * @note   初始化硬件驱动和页面管理器，注册所有页面
  */
void OLED_App_Init(void);

/**
  * @brief  按键中断回调处理（在HAL_GPIO_EXTI_Callback中调用）
  * @param  GPIO_Pin: 引脚编号
  */
void OLED_App_KeyCallback(uint16_t GPIO_Pin);

/**
  * @brief  主循环更新（可选，用于动画等需要定时刷新的场景）
  */
void OLED_App_Update(void);

#ifdef __cplusplus
}
#endif

#endif /* __OLED_APP_H__ */
