#ifndef __OLED_DIRECT_TEST_H__
#define __OLED_DIRECT_TEST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "hal_compat.h"
#include "i2c.h"

/**
  * @brief  直接测试OLED（绕过U8G2 CAD层）
  * @param  hi2c: I2C句柄
  * @param  addr: I2C地址
  * @retval HAL状态
  */
HAL_StatusTypeDef OLED_DirectTest(I2C_HandleTypeDef *hi2c, uint8_t addr);

#ifdef __cplusplus
}
#endif

#endif /* __OLED_DIRECT_TEST_H__ */
