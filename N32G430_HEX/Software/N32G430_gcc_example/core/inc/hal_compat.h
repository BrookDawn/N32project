/**
 * @file hal_compat.h
 * @brief Minimal HAL compatible definitions used by the OLED application layer.
 */

#ifndef HAL_COMPAT_H
#define HAL_COMPAT_H

#include "n32g430.h"
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    HAL_OK      = 0x00U,
    HAL_ERROR   = 0x01U,
    HAL_BUSY    = 0x02U,
    HAL_TIMEOUT = 0x03U
} HAL_StatusTypeDef;

typedef enum
{
    HAL_I2C_STATE_RESET   = 0x00U,
    HAL_I2C_STATE_READY   = 0x01U,
    HAL_I2C_STATE_BUSY    = 0x02U,
    HAL_I2C_STATE_BUSY_TX = 0x12U,
    HAL_I2C_STATE_ERROR   = 0x03U
} HAL_I2C_StateTypeDef;

typedef enum
{
    HAL_DMA_STATE_RESET = 0x00U,
    HAL_DMA_STATE_READY = 0x01U,
    HAL_DMA_STATE_BUSY  = 0x02U,
    HAL_DMA_STATE_ERROR = 0x03U
} HAL_DMA_StateTypeDef;

#define __HAL_RCC_DMA1_IS_CLK_ENABLED() ((RCC->AHBPCLKEN & RCC_AHBPCLKEN_DMAEN) != 0U)

void HAL_InitTick(void);
void HAL_IncTick(void);
uint32_t HAL_GetTick(void);
void HAL_Delay(uint32_t delay_ms);

#ifdef __cplusplus
}
#endif

#endif /* HAL_COMPAT_H */
