/**
 * @file dma.h
 * @brief Minimal DMA helper used by the OLED stack.
 */

#ifndef DMA_H
#define DMA_H

#include "hal_compat.h"
#include "n32g430_dma.h"

#ifdef __cplusplus
extern "C" {
#endif

struct __DMA_HandleTypeDef;

typedef void (*DMA_Callback)(struct __DMA_HandleTypeDef *hdma);

typedef struct __DMA_HandleTypeDef
{
    DMA_ChannelType *Instance;
    HAL_DMA_StateTypeDef State;
    uint8_t ChannelIndex;
    uint32_t RequestID;
    void *Parent;
    DMA_Callback XferCpltCallback;
    DMA_Callback XferErrorCallback;
} DMA_HandleTypeDef;

extern DMA_HandleTypeDef hdma_i2c1_tx;

void MX_DMA_Init(void);
void HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma);

#ifdef __cplusplus
}
#endif

#endif /* DMA_H */
