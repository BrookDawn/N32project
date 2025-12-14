#ifndef __OLED_SIMPLE_TEST_H__
#define __OLED_SIMPLE_TEST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "hal_compat.h"

/**
  * @brief  初始化OLED（简单测试）
  * @retval HAL状态
  */
HAL_StatusTypeDef OLED_SimpleTest_Init(void);

/**
  * @brief  清屏
  */
void OLED_SimpleTest_Clear(void);

/**
  * @brief  显示测试图案
  */
void OLED_SimpleTest_ShowPattern(void);

/**
  * @brief  全屏填充
  * @param  pattern: 填充图案（0x00=全黑，0xFF=全白）
  */
void OLED_SimpleTest_FillScreen(uint8_t pattern);

#ifdef __cplusplus
}
#endif

#endif /* __OLED_SIMPLE_TEST_H__ */
