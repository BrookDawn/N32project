/**
 * @file i2c.h
 * @brief Minimal I2C HAL-compatible interface for driving the OLED.
 */

#ifndef I2C_H
#define I2C_H

#include "hal_compat.h"
#include "n32g430_i2c.h"
#include "n32g430_gpio.h"
#include "dma.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    I2C_Module *Instance;
    HAL_I2C_StateTypeDef State;
    DMA_HandleTypeDef *hdmatx;
    uint8_t *pBuffPtr;
    uint16_t XferSize;
    uint16_t DevAddress;
} I2C_HandleTypeDef;

extern I2C_HandleTypeDef hi2c1;

void MX_I2C1_Init(void);
void HAL_I2C_DeInit(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c,
                                          uint16_t DevAddress,
                                          uint8_t *pData,
                                          uint16_t Size,
                                          uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Master_Transmit_DMA(I2C_HandleTypeDef *hi2c,
                                              uint16_t DevAddress,
                                              uint8_t *pData,
                                              uint16_t Size);

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c);

#ifdef __cplusplus
}
#endif

#endif /* I2C_H */
