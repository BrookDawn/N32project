/**
 * @file tim.h
 * @brief Minimal timer helpers for the OLED demo.
 */

#ifndef TIM_H
#define TIM_H

#include "hal_compat.h"
#include "n32g430_tim.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    TIM_Module *Instance;
    uint32_t Prescaler;
    uint32_t Period;
} TIM_HandleTypeDef;

extern TIM_HandleTypeDef htim4;

void MX_TIM4_Init(void);
HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *htim);
void HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim);

#ifndef __HAL_TIM_GET_COUNTER
#define __HAL_TIM_GET_COUNTER(__HANDLE__) ((__HANDLE__)->Instance->CNT)
#endif

#ifdef __cplusplus
}
#endif

#endif /* TIM_H */
